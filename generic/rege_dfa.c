/*
 * DFA routines
 * This file is #included by regexec.c.
 */

/*
 - longest - longest-preferred matching engine
 ^ static chr *longest(struct vars *, struct dfa *, chr *, chr *);
 */
static chr *			/* endpoint, or NULL */
longest(v, d, start, stop)
struct vars *v;			/* used only for debug and exec flags */
struct dfa *d;
chr *start;			/* where the match should start */
chr *stop;			/* match must end at or before here */
{
	chr *cp;
	chr *realstop = (stop == v->stop) ? stop : stop + 1;
	color co;
	struct sset *css;
	struct sset *ss;
	chr *post;
	int i;
	struct colormap *cm = d->cm;

	/* initialize */
	css = initialize(v, d, start);
	cp = start;

	/* startup */
	FDEBUG(("+++ startup +++\n"));
	if (cp == v->start) {
		co = d->cnfa->bos[(v->eflags&REG_NOTBOL) ? 0 : 1];
		FDEBUG(("color %ld\n", (long)co));
	} else {
		co = GETCOLOR(cm, *(cp - 1));
		FDEBUG(("char %c, color %ld\n", (char)*(cp-1), (long)co));
	}
	css = miss(v, d, css, co, cp, start);
	if (css == NULL)
		return NULL;
	css->lastseen = cp;

	/* main loop */
	if (v->eflags&REG_FTRACE)
		while (cp < realstop) {
			FDEBUG(("+++ at c%d +++\n", css - d->ssets));
			co = GETCOLOR(cm, *cp);
			FDEBUG(("char %c, color %ld\n", (char)*cp, (long)co));
			ss = css->outs[co];
			if (ss == NULL) {
				ss = miss(v, d, css, co, cp+1, start);
				if (ss == NULL)
					break;	/* NOTE BREAK OUT */
			}
			cp++;
			ss->lastseen = cp;
			css = ss;
		}
	else
		while (cp < realstop) {
			co = GETCOLOR(cm, *cp);
			ss = css->outs[co];
			if (ss == NULL) {
				ss = miss(v, d, css, co, cp+1, start);
				if (ss == NULL)
					break;	/* NOTE BREAK OUT */
			}
			cp++;
			ss->lastseen = cp;
			css = ss;
		}

	/* shutdown */
	FDEBUG(("+++ shutdown at c%d +++\n", css - d->ssets));
	if (cp == v->stop && stop == v->stop) {
		co = d->cnfa->eos[(v->eflags&REG_NOTEOL) ? 0 : 1];
		FDEBUG(("color %ld\n", (long)co));
		ss = miss(v, d, css, co, cp, start);
		/* special case:  match ended at eol? */
		if (ss != NULL && (ss->flags&POSTSTATE))
			return cp;
		else if (ss != NULL)
			ss->lastseen = cp;	/* to be tidy */
	}

	/* find last match, if any */
	post = d->lastpost;
	for (ss = d->ssets, i = 0; i < d->nssused; ss++, i++)
		if ((ss->flags&POSTSTATE) && post != ss->lastseen &&
					(post == NULL || post < ss->lastseen))
			post = ss->lastseen;
	if (post != NULL)		/* found one */
		return post - 1;

	return NULL;
}

/*
 - shortest - shortest-preferred matching engine
 ^ static chr *shortest(struct vars *, struct dfa *, chr *, chr *, chr *,
 ^ 	chr **);
 */
static chr *			/* endpoint, or NULL */
shortest(v, d, start, min, max, coldp)
struct vars *v;			/* used only for debug and exec flags */
struct dfa *d;
chr *start;			/* where the match should start */
chr *min;			/* match must end at or after here */
chr *max;			/* match must end at or before here */
chr **coldp;			/* store coldstart pointer here, if nonNULL */
{
	chr *cp;
	chr *realmin = (min == v->stop) ? min : min + 1;
	chr *realmax = (max == v->stop) ? max : max + 1;
	color co;
	struct sset *css;
	struct sset *firstss;
	struct sset *ss;
	struct colormap *cm = d->cm;

	/* initialize */
	css = initialize(v, d, start);
	firstss = css;
	cp = start;

	/* startup */
	FDEBUG(("--- startup ---\n"));
	if (cp == v->start) {
		co = d->cnfa->bos[(v->eflags&REG_NOTBOL) ? 0 : 1];
		FDEBUG(("color %ld\n", (long)co));
	} else {
		co = GETCOLOR(cm, *(cp - 1));
		FDEBUG(("char %c, color %ld\n", (char)*(cp-1), (long)co));
	}
	css = miss(v, d, css, co, cp, start);
	if (css == NULL)
		return NULL;
	css->lastseen = cp;
	ss = css;

	/* main loop */
	if (v->eflags&REG_FTRACE)
		while (cp < realmax) {
			FDEBUG(("--- at c%d ---\n", css - d->ssets));
			co = GETCOLOR(cm, *cp);
			FDEBUG(("char %c, color %ld\n", (char)*cp, (long)co));
			ss = css->outs[co];
			if (ss == NULL) {
				ss = miss(v, d, css, co, cp+1, start);
				if (ss == NULL)
					break;	/* NOTE BREAK OUT */
			}
			cp++;
			ss->lastseen = cp;
			css = ss;
			if ((ss->flags&POSTSTATE) && cp >= realmin)
				break;		/* NOTE BREAK OUT */
		}
	else
		while (cp < realmax) {
			co = GETCOLOR(cm, *cp);
			ss = css->outs[co];
			if (ss == NULL) {
				ss = miss(v, d, css, co, cp+1, start);
				if (ss == NULL)
					break;	/* NOTE BREAK OUT */
			}
			cp++;
			ss->lastseen = cp;
			css = ss;
			if ((ss->flags&POSTSTATE) && cp >= realmin)
				break;		/* NOTE BREAK OUT */
		}

	if (ss == NULL)
		return NULL;
	if (ss->flags&POSTSTATE) {
		assert(firstss->flags&STARTER);
		assert(firstss->lastseen != NULL);
		if (coldp != NULL)
			*coldp = firstss->lastseen;
		assert(cp >= realmin);
		return cp - 1;
	}

	/* shutdown */
	FDEBUG(("--- shutdown at c%d ---\n", css - d->ssets));
	if (cp == v->stop && max == v->stop) {
		co = d->cnfa->eos[(v->eflags&REG_NOTEOL) ? 0 : 1];
		FDEBUG(("color %ld\n", (long)co));
		ss = miss(v, d, css, co, cp, start);
		/* special case:  match ended at eol? */
		if (ss != NULL && (ss->flags&POSTSTATE)) {
			assert(firstss->flags&STARTER);
			assert(firstss->lastseen != NULL);
			if (coldp != NULL)
				*coldp = firstss->lastseen;
			return cp;
		}
	}

	return NULL;
}

/*
 - newdfa - set up a fresh DFA
 ^ static struct dfa *newdfa(struct vars *, struct cnfa *,
 ^ 	struct colormap *);
 */
static struct dfa *
newdfa(v, cnfa, cm)
struct vars *v;
struct cnfa *cnfa;
struct colormap *cm;
{
	struct dfa *d = (struct dfa *)MALLOC(sizeof(struct dfa));
	int wordsper = (cnfa->nstates + UBITS - 1) / UBITS;
	struct sset *ss;
	int i;

	assert(cnfa != NULL && cnfa->nstates != 0);
	if (d == NULL) {
		ERR(REG_ESPACE);
		return NULL;
	}

	d->ssets = (struct sset *)MALLOC(CACHE * sizeof(struct sset));
	d->statesarea = (unsigned *)MALLOC((CACHE+WORK) * wordsper *
							sizeof(unsigned));
	d->work = &d->statesarea[CACHE * wordsper];
	d->outsarea = (struct sset **)MALLOC(CACHE * cnfa->ncolors *
							sizeof(struct sset *));
	d->incarea = (struct arcp *)MALLOC(CACHE * cnfa->ncolors *
							sizeof(struct arcp));
	if (d->ssets == NULL || d->statesarea == NULL || d->outsarea == NULL ||
							d->incarea == NULL) {
		freedfa(d);
		ERR(REG_ESPACE);
		return NULL;
	}

	d->nssets = (v->eflags&REG_SMALL) ? 7 : CACHE;
	d->nssused = 0;
	d->nstates = cnfa->nstates;
	d->ncolors = cnfa->ncolors;
	d->wordsper = wordsper;
	d->cnfa = cnfa;
	d->cm = cm;
	d->lastpost = NULL;
	d->search = d->ssets;

	for (ss = d->ssets, i = 0; i < d->nssets; ss++, i++) {
		/* initialization of most fields is done as needed */
		ss->states = &d->statesarea[i * d->wordsper];
		ss->outs = &d->outsarea[i * d->ncolors];
		ss->inchain = &d->incarea[i * d->ncolors];
	}

	return d;
}

/*
 - freedfa - free a DFA
 ^ static VOID freedfa(struct dfa *);
 */
static VOID
freedfa(d)
struct dfa *d;
{
	if (d->ssets != NULL)
		FREE(d->ssets);
	if (d->statesarea != NULL)
		FREE(d->statesarea);
	if (d->outsarea != NULL)
		FREE(d->outsarea);
	if (d->incarea != NULL)
		FREE(d->incarea);
	FREE(d);
}

/*
 - hash - construct a hash code for a bitvector
 * There are probably better ways, but they're more expensive.
 ^ static unsigned hash(unsigned *, int);
 */
static unsigned
hash(uv, n)
unsigned *uv;
int n;
{
	int i;
	unsigned h;

	h = 0;
	for (i = 0; i < n; i++)
		h ^= uv[i];
	return h;
}

/*
 - initialize - hand-craft a cache entry for startup, otherwise get ready
 ^ static struct sset *initialize(struct vars *, struct dfa *, chr *);
 */
static struct sset *
initialize(v, d, start)
struct vars *v;			/* used only for debug flags */
struct dfa *d;
chr *start;
{
	struct sset *ss;
	int i;

	/* is previous one still there? */
	if (d->nssused > 0 && (d->ssets[0].flags&STARTER))
		ss = &d->ssets[0];
	else {				/* no, must (re)build it */
		ss = getvacant(v, d, start, start);
		for (i = 0; i < d->wordsper; i++)
			ss->states[i] = 0;
		BSET(ss->states, d->cnfa->pre);
		ss->hash = hash(ss->states, d->wordsper);
		assert(d->cnfa->pre != d->cnfa->post);
		ss->flags = STARTER|LOCKED;
		/* lastseen dealt with below */
	}

	for (i = 0; i < d->nssused; i++)
		d->ssets[i].lastseen = NULL;
	ss->lastseen = start;		/* maybe untrue, but harmless */
	d->lastpost = NULL;
	return ss;
}

/*
 - miss - handle a cache miss
 ^ static struct sset *miss(struct vars *, struct dfa *, struct sset *,
 ^ 	pcolor, chr *, chr *);
 */
static struct sset *		/* NULL if goes to empty set */
miss(v, d, css, co, cp, start)
struct vars *v;			/* used only for debug flags */
struct dfa *d;
struct sset *css;
pcolor co;
chr *cp;			/* next chr */
chr *start;			/* where the attempt got started */
{
	struct cnfa *cnfa = d->cnfa;
	int i;
	unsigned h;
	struct carc *ca;
	struct sset *p;
	int ispost;
	int gotstate;
	int dolacons;
	int didlacons;

	/* for convenience, we can be called even if it might not be a miss */
	if (css->outs[co] != NULL) {
		FDEBUG(("hit\n"));
		return css->outs[co];
	}
	FDEBUG(("miss\n"));

	/* first, what set of states would we end up in? */
	for (i = 0; i < d->wordsper; i++)
		d->work[i] = 0;
	ispost = 0;
	gotstate = 0;
	for (i = 0; i < d->nstates; i++)
		if (ISBSET(css->states, i))
			for (ca = cnfa->states[i]; ca->co != COLORLESS; ca++)
				if (ca->co == co) {
					BSET(d->work, ca->to);
					gotstate = 1;
					if (ca->to == cnfa->post)
						ispost = 1;
					FDEBUG(("%d -> %d\n", i, ca->to));
				}
	dolacons = (gotstate) ? (cnfa->flags&HASLACONS) : 0;
	didlacons = 0;
	while (dolacons) {		/* transitive closure */
		dolacons = 0;
		for (i = 0; i < d->nstates; i++)
			if (ISBSET(d->work, i))
				for (ca = cnfa->states[i]; ca->co != COLORLESS;
									ca++)
					if (ca->co > cnfa->ncolors &&
						!ISBSET(d->work, ca->to) &&
							lacon(v, cnfa, cp,
								ca->co)) {
						BSET(d->work, ca->to);
						dolacons = 1;
						didlacons = 1;
						if (ca->to == cnfa->post)
							ispost = 1;
						FDEBUG(("%d :> %d\n",i,ca->to));
					}
	}
	if (!gotstate)
		return NULL;
	h = hash(d->work, d->wordsper);

	/* next, is that in the cache? */
	for (p = d->ssets, i = d->nssused; i > 0; p++, i--)
		if (p->hash == h && memcmp(VS(d->work), VS(p->states),
					d->wordsper*sizeof(unsigned)) == 0) {
			FDEBUG(("cached c%d\n", p - d->ssets));
			break;			/* NOTE BREAK OUT */
		}
	if (i == 0) {		/* nope, need a new cache entry */
		p = getvacant(v, d, cp, start);
		assert(p != css);
		for (i = 0; i < d->wordsper; i++)
			p->states[i] = d->work[i];
		p->hash = h;
		p->flags = (ispost) ? POSTSTATE : 0;
		/* lastseen to be dealt with by caller */
	}

	if (!didlacons) {		/* lookahead conds. always cache miss */
		css->outs[co] = p;
		css->inchain[co] = p->ins;
		p->ins.ss = css;
		p->ins.co = (color)co;
	}
	return p;
}

/*
 - lacon - lookahead-constraint checker for miss()
 ^ static int lacon(struct vars *, struct cnfa *, chr *, pcolor);
 */
static int			/* predicate:  constraint satisfied? */
lacon(v, pcnfa, cp, co)
struct vars *v;
struct cnfa *pcnfa;		/* parent cnfa */
chr *cp;
pcolor co;			/* "color" of the lookahead constraint */
{
	int n;
	struct subre *sub;
	struct dfa *d;
	chr *end;

	n = co - pcnfa->ncolors;
	assert(n < v->g->nlacons && v->g->lacons != NULL);
	FDEBUG(("=== testing lacon %d\n", n));
	sub = &v->g->lacons[n];
	d = newdfa(v, &sub->cnfa, &v->g->cmap);
	if (d == NULL) {
		ERR(REG_ESPACE);
		return 0;
	}
	end = longest(v, d, cp, v->stop);
	freedfa(d);
	FDEBUG(("=== lacon %d match %d\n", n, (end != NULL)));
	return (sub->subno) ? (end != NULL) : (end == NULL);
}

/*
 - getvacant - get a vacant state set
 * This routine clears out the inarcs and outarcs, but does not otherwise
 * clear the innards of the state set -- that's up to the caller.
 ^ static struct sset *getvacant(struct vars *, struct dfa *, chr *, chr *);
 */
static struct sset *
getvacant(v, d, cp, start)
struct vars *v;			/* used only for debug flags */
struct dfa *d;
chr *cp;
chr *start;
{
	int i;
	struct sset *ss;
	struct sset *p;
	struct arcp ap;
	struct arcp lastap;
	color co;

	ss = pickss(v, d, cp, start);
	assert(!(ss->flags&LOCKED));

	/* clear out its inarcs, including self-referential ones */
	ap = ss->ins;
	while ((p = ap.ss) != NULL) {
		co = ap.co;
		FDEBUG(("zapping c%d's %ld outarc\n", p - d->ssets, (long)co));
		p->outs[co] = NULL;
		ap = p->inchain[co];
		p->inchain[co].ss = NULL;	/* paranoia */
	}
	ss->ins.ss = NULL;

	/* take it off the inarc chains of the ssets reached by its outarcs */
	for (i = 0; i < d->ncolors; i++) {
		p = ss->outs[i];
		assert(p != ss);		/* not self-referential */
		if (p == NULL)
			continue;		/* NOTE CONTINUE */
		FDEBUG(("del outarc %d from c%d's in chn\n", i, p - d->ssets));
		if (p->ins.ss == ss && p->ins.co == i)
			p->ins = ss->inchain[i];
		else {
			assert(p->ins.ss != NULL);
			for (ap = p->ins; ap.ss != NULL &&
						!(ap.ss == ss && ap.co == i);
						ap = ap.ss->inchain[ap.co])
				lastap = ap;
			assert(ap.ss != NULL);
			lastap.ss->inchain[lastap.co] = ss->inchain[i];
		}
		ss->outs[i] = NULL;
		ss->inchain[i].ss = NULL;
	}

	/* if ss was a success state, may need to remember location */
	if ((ss->flags&POSTSTATE) && ss->lastseen != d->lastpost &&
			(d->lastpost == NULL || d->lastpost < ss->lastseen))
		d->lastpost = ss->lastseen;

	return ss;
}

/*
 - pickss - pick the next stateset to be used
 ^ static struct sset *pickss(struct vars *, struct dfa *, chr *, chr *);
 */
static struct sset *
pickss(v, d, cp, start)
struct vars *v;			/* used only for debug flags */
struct dfa *d;
chr *cp;
chr *start;
{
	int i;
	struct sset *ss;
	struct sset *end;
	chr *ancient;

	/* shortcut for cases where cache isn't full */
	if (d->nssused < d->nssets) {
		ss = &d->ssets[d->nssused];
		d->nssused++;
		FDEBUG(("new c%d\n", ss - d->ssets));
		/* must make innards consistent */
		ss->ins.ss = NULL;
		ss->ins.co = WHITE;		/* give it some value */
		for (i = 0; i < d->ncolors; i++) {
			ss->outs[i] = NULL;
			ss->inchain[i].ss = NULL;
		}
		ss->flags = 0;
		return ss;
	}

	/* look for oldest, or old enough anyway */
	if (cp - start > d->nssets*2/3)		/* oldest 33% are expendable */
		ancient = cp - d->nssets*2/3;
	else
		ancient = start;
	for (ss = d->search, end = &d->ssets[d->nssets]; ss < end; ss++)
		if ((ss->lastseen == NULL || ss->lastseen < ancient) &&
							!(ss->flags&LOCKED)) {
			d->search = ss + 1;
			FDEBUG(("replacing c%d\n", ss - d->ssets));
			return ss;
		}
	for (ss = d->ssets, end = d->search; ss < end; ss++)
		if ((ss->lastseen == NULL || ss->lastseen < ancient) &&
							!(ss->flags&LOCKED)) {
			d->search = ss + 1;
			FDEBUG(("replacing c%d\n", ss - d->ssets));
			return ss;
		}

	/* nobody's old enough?!? -- something's really wrong */
	FDEBUG(("can't find victim to replace!\n"));
	assert(NOTREACHED);
	ERR(REG_ASSERT);
	return d->ssets;
}
