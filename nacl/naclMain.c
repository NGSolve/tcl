#include <stdlib.h>
#include <string.h>
#include <ppapi/c/dev/ppb_var_deprecated.h>
#include <ppapi/c/dev/ppp_class_deprecated.h>
#include <ppapi/c/pp_errors.h>
#include <ppapi/c/pp_var.h>
#include <ppapi/c/pp_module.h>
#include <ppapi/c/ppb.h>
#include <ppapi/c/ppb_instance.h>
#include <ppapi/c/ppp.h>
#include <ppapi/c/ppp_instance.h>


#include <errno.h>
#include "tcl.h"

static Tcl_Interp *interp = NULL;

const char *init_tcl_contents =
"set ::tcl_library {/}\n"
# include "init.tcl.c"
  ;

static Tcl_Interp *NewTcl(void)
{
  Tcl_Interp *ii;

  printf("DBUG:TclInitSubsystems\n");
  TclInitSubsystems();
  printf("DBUG:TclpSetInitialEncodings\n");
  TclpSetInitialEncodings();

  printf("DBUG:Busyloop-check\n");
  {
    volatile int x;

    x=0;
    if (x) printf("BUSYLOOP : break out by resetting the var (set $eax=0)\n");
    else printf("NO-BUSYLOOP\n");
    while(x) {}
  }
  printf("DBUG:CreateInterp\n");
  ii=Tcl_CreateInterp();
  if (!ii) {
    printf("Tcl CreateInterp Failed !!!\n");
    return NULL;
  }
  printf("DBUG:Tcl_Init2\n");
  if (Tcl_Eval(ii,init_tcl_contents)!=TCL_OK) {
    printf("Tcl Init Failed: %s !!!\n",Tcl_GetStringResult(ii));
    return NULL;
  }
  return ii;
}
static char *EvalTcl(char *s)
{
  if (!interp) return "No Tcl Interp!!!";
  Tcl_Eval(interp,s);
  return Tcl_GetStringResult(interp);
}


static PP_Bool Instance_DidCreate(PP_Instance instance,
                                  uint32_t argc,
                                  const char* argn[],
                                  const char* argv[]);
static void Instance_DidDestroy(PP_Instance instance);
static void Instance_DidChangeView(PP_Instance instance,
                                   const struct PP_Rect* position,
                                   const struct PP_Rect* clip);
static void Instance_DidChangeFocus(PP_Instance instance,
                                    PP_Bool has_focus);
static PP_Bool Instance_HandleInputEvent(PP_Instance instance,
                                         const struct PP_InputEvent* event);
static struct PP_Var Instance_GetInstanceObject(PP_Instance instance);

static PP_Module module_id = 0;
static struct PPB_Var_Deprecated* var_interface = NULL;
static struct PPP_Class_Deprecated ppp_class;
static struct PPP_Instance instance_interface = {
  &Instance_DidCreate,
  &Instance_DidDestroy,
  &Instance_DidChangeView,
  &Instance_DidChangeFocus,
  &Instance_HandleInputEvent,
  NULL,  /* HandleDocumentLoad is not supported by NaCl modules. */
  &Instance_GetInstanceObject,
};


/**
 * Returns C string contained in the @a var or NULL if @a var is not string.
 * @param[in] var PP_Var containing string.
 * @return a C string representation of @a var.
 * @note Returned pointer will be invalid after destruction of @a var.
 */
static const char* VarToCStr(struct PP_Var var) {
  uint32_t len = 0;
  if (NULL != var_interface)
    return var_interface->VarToUtf8(var, &len);
  return NULL;
}

/**
 * Creates new string PP_Var from C string. The resulting object will be a
 * refcounted string object. It will be AddRef()ed for the caller. When the
 * caller is done with it, it should be Release()d.
 * @param[in] str C string to be converted to PP_Var
 * @return PP_Var containing string.
 */
static struct PP_Var StrToVar(const char* str) {
  if (NULL != var_interface)
    return var_interface->VarFromUtf8(module_id, str, strlen(str));
  return PP_MakeUndefined();
}

/**
 * A simple function that always returns 42.
 * @return always returns the integer 42
 */
static struct PP_Var FortyTwo() {
  return PP_MakeInt32(42);
}

/**
 * Called when the NaCl module is instantiated on the web page. The identifier
 * of the new instance will be passed in as the first argument (this value is
 * generated by the browser and is an opaque handle).  This is called for each
 * instantiation of the NaCl module, which is each time the <embed> tag for
 * this module is encountered.
 *
 * If this function reports a failure (by returning @a PP_FALSE), the NaCl
 * module will be deleted and DidDestroy will be called.
 * @param[in] instance The identifier of the new instance representing this
 *     NaCl module.
 * @param[in] argc The number of arguments contained in @a argn and @a argv.
 * @param[in] argn An array of argument names.  These argument names are
 *     supplied in the <embed> tag, for example:
 *       <embed id="nacl_module" dimensions="2">
 *     will produce two arguments, one named "id" and one named "dimensions".
 * @param[in] argv An array of argument values.  These are the values of the
 *     arguments listed in the <embed> tag.  In the above example, there will
 *     be two elements in this array, "nacl_module" and "2".  The indices of
 *     these values match the indices of the corresponding names in @a argn.
 * @return @a PP_TRUE on success.
 */
static PP_Bool Instance_DidCreate(PP_Instance instance,
                                  uint32_t argc,
                                  const char* argn[],
                                  const char* argv[]) {
  return PP_TRUE;
}

/**
 * Called when the NaCl module is destroyed. This will always be called,
 * even if DidCreate returned failure. This routine should deallocate any data
 * associated with the instance.
 * @param[in] instance The identifier of the instance representing this NaCl
 *     module.
 */
static void Instance_DidDestroy(PP_Instance instance) {
}

/**
 * Called when the position, the size, or the clip rect of the element in the
 * browser that corresponds to this NaCl module has changed.
 * @param[in] instance The identifier of the instance representing this NaCl
 *     module.
 * @param[in] position The location on the page of this NaCl module. This is
 *     relative to the top left corner of the viewport, which changes as the
 *     page is scrolled.
 * @param[in] clip The visible region of the NaCl module. This is relative to
 *     the top left of the plugin's coordinate system (not the page).  If the
 *     plugin is invisible, @a clip will be (0, 0, 0, 0).
 */
static void Instance_DidChangeView(PP_Instance instance,
                                   const struct PP_Rect* position,
                                   const struct PP_Rect* clip) {
}

/**
 * Notification that the given NaCl module has gained or lost focus.
 * Having focus means that keyboard events will be sent to the NaCl module
 * represented by @a instance. A NaCl module's default condition is that it
 * will not have focus.
 *
 * Note: clicks on NaCl modules will give focus only if you handle the
 * click event. You signal if you handled it by returning @a true from
 * HandleInputEvent. Otherwise the browser will bubble the event and give
 * focus to the element on the page that actually did end up consuming it.
 * If you're not getting focus, check to make sure you're returning true from
 * the mouse click in HandleInputEvent.
 * @param[in] instance The identifier of the instance representing this NaCl
 *     module.
 * @param[in] has_focus Indicates whether this NaCl module gained or lost
 *     event focus.
 */
static void Instance_DidChangeFocus(PP_Instance instance,
                                    PP_Bool has_focus) {
}

/**
 * General handler for input events. Returns true if the event was handled or
 * false if it was not.
 *
 * If the event was handled, it will not be forwarded to the web page or
 * browser. If it was not handled, it will bubble according to the normal
 * rules. So it is important that the NaCl module respond accurately with
 * whether event propogation should continue.
 *
 * Event propogation also controls focus. If you handle an event like a mouse
 * event, typically your NaCl module will be given focus. Returning false means
 * that the click will be given to a lower part of the page and your NaCl
 * module will not receive focus. This allows a plugin to be partially
 * transparent, where clicks on the transparent areas will behave like clicks
 * to the underlying page.
 * @param[in] instance The identifier of the instance representing this NaCl
 *     module.
 * @param[in] event The event.
 * @return PP_TRUE if @a event was handled, PP_FALSE otherwise.
 */
static PP_Bool Instance_HandleInputEvent(PP_Instance instance,
                                         const struct PP_InputEvent* event) {
  /* We don't handle any events. */
  return PP_FALSE;
}

/**
 * Create scriptable object for the given instance.
 * @param[in] instance The instance ID.
 * @return A scriptable object.
 */
static struct PP_Var Instance_GetInstanceObject(PP_Instance instance) {
  if (var_interface)
    return var_interface->CreateObject(instance, &ppp_class, NULL);
  return PP_MakeUndefined();
}

/**
 * Check existence of the function associated with @a name.
 * @param[in] object unused
 * @param[in] name method name
 * @param[out] exception pointer to the exception object, unused
 * @return If the method does exist, return true.
 * If the method does not exist, return false and don't set the exception.
 */
static bool Tcl_HasMethod(void* object,
                                 struct PP_Var name,
                                 struct PP_Var* exception) {
  const char* method_name = VarToCStr(name);
  if (NULL != method_name) {
    if (strcmp(method_name, "eval") == 0)
      return true;
  }
  return false;
}

/**
 * Invoke the function associated with @a name.
 * @param[in] object unused
 * @param[in] name method name
 * @param[in] argc number of arguments
 * @param[in] argv array of arguments
 * @param[out] exception pointer to the exception object, unused
 * @return If the method does exist, return true.
 */
static struct PP_Var Tcl_Call(void* object,
                                     struct PP_Var name,
                                     uint32_t argc,
                                     struct PP_Var* argv,
                                     struct PP_Var* exception) {
  struct PP_Var v = PP_MakeUndefined();
  const char* method_name = VarToCStr(name);
  if (NULL != method_name) {
    if (strcmp(method_name, "eval") == 0) {
      if (argc == 1) {
        if (argv[0].type != PP_VARTYPE_STRING) {
          v = StrToVar("Arg from Javascript is not a string!");
        } else {
          char* str = strdup(VarToCStr(argv[0]));
	  char* res = EvalTcl(str);
          v = StrToVar(res);
          free(str);
        }
      } else {
        v = StrToVar("Unexpected number of args");
      }
    } else {
      v = StrToVar("Unknown method");
    }
  }
  return v;
}

/**
 * Entrypoints for the module.
 * Initialize instance interface and scriptable object class.
 * @param[in] a_module_id module ID
 * @param[in] get_browser pointer to PPB_GetInterface
 * @return PP_OK on success, any other value on failure.
 */
PP_EXPORT int32_t PPP_InitializeModule(PP_Module a_module_id,
                                       PPB_GetInterface get_browser) {
  module_id = a_module_id;
  var_interface =
      (struct PPB_Var_Deprecated*)(get_browser(PPB_VAR_DEPRECATED_INTERFACE));

  printf("DBUG:begin\n");
  interp = NewTcl();
  if (!interp) return PP_ERROR_FAILED;

  memset(&ppp_class, 0, sizeof(ppp_class));
  ppp_class.Call = Tcl_Call;
  ppp_class.HasMethod = Tcl_HasMethod;
  return PP_OK;
}

/**
 * Returns an interface pointer for the interface of the given name, or NULL
 * if the interface is not supported.
 * @param[in] interface_name name of the interface
 * @return pointer to the interface
 */
PP_EXPORT const void* PPP_GetInterface(const char* interface_name) {
  if (strcmp(interface_name, PPP_INSTANCE_INTERFACE) == 0)
    return &instance_interface;
  return NULL;
}

/**
 * Called before the plugin module is unloaded.
 */
PP_EXPORT void PPP_ShutdownModule() {
  //  if (interp) EndTcl(interp);
}
