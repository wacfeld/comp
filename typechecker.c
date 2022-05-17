
// // decls contain lost of nested pointers. we use a function to make a deep copy so we can modify
// decl *copydecl(decl *ct)
// {
//   // first copy superficial info in
//   decl *newct = malloc(sizeof(decl));
//   memcpy(newct, ct, sizeof(decl));

//   // make deep copy of sets/lists

//   newct->typespecs = copyset(ct->typespecs);
//   newct->typequals = copyset(ct->typequals);

//   newct->typemods = copyset(ct->typemods); // copyset works on lists too

// }


// // get the size of a primary expression (by passing its token)
// // primary expressions can be identifiers, string literals, constants, or (expression)
// // (expression) will never occur because it evaluates to another kind of expression
// // TODO enums, etc. should be in scope
// int sizeofprim(token *t, decl **scope, int sn)
// {
//   int type = t->gen.type;

//   if(type == FLOATING)
//     return FLOAT_SIZE;
//   if(type == INTEGER)
//     return INT_SIZE;
//   if(type == CHAR)
//     return CHAR_SIZE;

//   if(type == IDENT)
//   {
//     // look up identifier in list of declarations
//     char *ident = t->ident.cont;
//     for(int i = sn-1; i >= 0; i--) // we go backwards, so that inner block declarations "mask" outer ones
//     {
//       if(streq(scope[i]->ident, ident)) // found the thing
//       {
//         return sizeofdecl(scope[i]);
//       }
//     }

//     // if for loop exits, we failed to find the identifier
//     assert(!"could not find identifier");
//   }

//   if(type == STRLIT)
//   {
//     // return length of string including added-on \0
//     return t->strlit.len;
//   }
// }


// int typeofprim(token *t, decl **scope, int sn)
// {
//   int type == t->gen.type;
//   if(type == FLOATING)
//   {
//     decl *ct = calloc(1, sizeof(decl));
//     ct->storespec = -1;
    
//     if(t->floating.isshort)
//       ct->dattype = FLOAT_T;
//     else if(t->floating.islong)
//       ct->dattype = LDUB_T;
//     else
//       ct->dattype = DUB_T;


//     return ct;
//   }

//   if(type == INTEGER)
//   {
//     decl *ct = calloc(1, sizeof(decl));
//     ct->storespec = -1;
    
//     if(t->integer.isdecimal && (!t->integer.islong) && (!t->integer.isunsigned)) // decimal and unsuffixed
//     {
//       if(t->integer.cont <= INT_MAX)
//         ct->dattype = INT_T;
//       // long int is same size as int in our implementation. skip
//       else
//         ct->dattype = ULINT_T;
//     }

//     else if(!t->integer.isdecimal && (!t->integer.islong) && (!t->integer.isunsigned)) // unsuffixed, not decimal
//     {
//       if(t->integer.cont <= INT_MAX)
//         ct->dattype = INT_T;
//       // long int is same size as int in our implementation. skip
//       else
//         ct->dattype = UINT_T;
//     }

//     else if(t->integer.isunsigned && (!t->integer.islong))
//     {
//       // always this under our implementation
//       ct->dattype = UINT_T;
//     }

//     else if(t->integer.islong)
//     {
//       if(t->integer.cont <= INT_MAX)
//         ct->dattype = LINT_T;
//       // long int is same size as int in our implementation. skip
//       else
//         ct->dattype = ULINT_T;
//     }

//     else if(t->integer.islong && t->integer.isunsigned)
//     {
//       ct->dattype = ULINT_T;
//     }

//     return ct;
//   }

//   if(type == IDENT)
//   {
//     // look up identifier in list of declarations
//     char *ident = t->ident.cont;
//     for(int i = sn-1; i >= 0; i--) // we go backwards, so that inner block declarations "mask" outer ones
//     {
//       if(streq(scope[i]->ident, ident)) // found the thing
//       {
//         return scope[i];
//       }
//     }

//     // if for loop exits, we failed to find the identifier
//     assert(!"could not find identifier");
//   }

//   if(type == CHAR)
//   {
//     decl *ct = calloc(1, sizeof(decl));
//     ct->storespec = -1;
//     ct->dattype = CHAR_T;
//   }

//   if(type == STRLIT)
//   {
//     decl *ct = calloc(1, sizeof(decl));
//     ct->storespec = -1;
    
//     // chars
//     ct->dattype = CHAR_T;
//     ct->typemods = makelist(sizeof(typemod));

//     // array of
//     typemod tm;
//     tm.gen.type = TM_ARR;
//     tm.arr.len = t->strlit.len;
//     append(ct->typemods, &tm);

//     return ct;
//   }
// }

// TODO integral promotion (p. 174)
// TODO pointer conversion (p. 177)

// int isarith(decl *ct)
// {
//   int t = ct->type;
//   if(ct->typemods && ct->typemods->n > 0) // typespec is not toplevel, there are typemods
//     return 0;
//   // TODO enums (should be dealt with in isintegral() actually
//   return isintegral(ct) || t == FLOAT_T || t == DUB_T || t == LDUB_T;
// }

// int isintegral(decl *ct)
// {
//   int t = ct->type;
//   if(ct->typemods && ct->typemods->n > 0) // typespec is not toplevel, there are typemods
//     return 0;
//   // TODO enums
//   return t == CHAR_T || t == UCHAR_T || t == INT_T || t == UINT_T || t == SINT_T || t == LINT_T || t == USINT_T || t == ULINT_T;
// }

// // perform integral promotion on integer decl
// // returns dattype
// // TODO figure out
// int intpromote(int dt, u_int32_t x)
// {
//   // TODO enums (which are ints)
//   if(dt == CHAR_T || dt == SINT_T || dt == USINT_T || dt == UCHAR_T)
//   {
//     if(x <= INT_MAX) // fits in signed int
//       return INT_T;
//     else
//       return UINT_T;
//   }

//   else // otherwise don't change
//     return dt;
// }

// // get first typemod of type. if no typemods, return -1
// typemod *gettm(decl *ct, int i)
// {
//   if(!ct->typemods) return NULL; // typemods don't exist
//   if(!ct->typemods->n) return NULL; // typemods has len 0

//   assert(i < ct->typemods->n);
//   typemod *tms = (typemod *) ct->typemods->cont;
//   returnt tms + i;
// }

// decl *getexprtype(expr *e, decl **scope, int sn)
// {
//   // TODO pointer conversion, for functions and arrays
  
//   // if ct already there, use
//   if(e->ct)
//     return e->ct;
  
//   if(e->type == PRIM_E)
//   {
//     decl *ct = typeofprim(e->tok, scope, sn); // get type
//     e->ct = ct; // remember type
//     return ct; // return
//   }

//   if(e->type == POST_E)
//   {
//     if(e->optype == ARR_O)
//     {
//       // recurse to get subtypes
//       decl *ct1 = getexprtype(e->args[0], scope, sn);
//       decl *ct2 = getexprtype(e->args[1], scope, sn);
      
//       // get typemods (NULL if doesn't exist)
//       typemod *tm1 = gettm(ct1, 0);
//       typemod *tm2 = gettm(ct2, 0);
      
//       // pointer[integral] and integral[pointer] are the two valid forms
//       if(tm1 && tm1->gen.type == TM_PTR && isintegral(ct2))
//       {
//         // pointer to T -> return type T
//         decl *ct = malloc(sizeof(decl));
//         memcpy(ct, ct1, sizeof(decl)); // pointer to T
//         // rem_front(ct->typemods); // T
//         // can't use rem_front because our pointers are pointing to the same list
//         // ct->typemods->cont += ct->typemods->size; // T
//         shift_front(ct->typemods);

//         // attach to e and return
//         e->ct = ct;
//         return ct;
//       }
//       if(isintegral(ct1) && tm2 && tm2->gen.type == TM_PTR)
//       {
//         decl *ct = malloc(sizeof(decl));
//         memcpy(ct, ct2, sizeof(decl));
//         // rem_front(ct->typemods);
//         // ct->typemods->cont += ct->typemods->size; // T
//         shift_front(ct->typemods);

//         e->ct = ct;
//         return ct;
//       }

//       assert(!"invalid array indexing");
//     }

//     if(e->optype == FUN_O)
//     {
//       decl *ct1 = getexprtype(e->args[0], scope, sn); // pointer to function
//       assert(e->args[1]->type == ARGLIST); // arglist

//       typemod *tm1 = gettm(ct1, 0);
//       assert(tm1->gen.type == TM_PTR); // pointer to
//       tm1 = gettm(ct1, 1);
//       assert(tm2->gen.type == TM_FUNC); // function

//       list *params = tm1->func.params;
//       if(params) // if params specified, go through and check them against the arglist
//       {
//         assert(params->n == e->args[1]->numargs);
//         decl *paramtypes = (decl *) params->cont;
//         for(int i = 0; i < params->n; i++)
//         {
//           decl *ct = getexprtype(e->args[1]->args[i], scope, sn);
//           assert(iscompat(paramtypes[i], ct, 1, 1)); // toplevel and assignment flags on
//         }
//       }

//       decl *ct = malloc(sizeof(decl));
//       memcpy(ct, ct1, sizeof(decl));
//       // rem_front(ct->typemods); // * () T -> () T
//       // rem_front(ct->typemods); // () T -> T
//       // ct->typemods->cont += ct->typemods->size * 2; // * () T -> T
//       shift_front(ct->typemods); // () T
//       shift_front(ct->typemods); // T

//       return ct;
//     }

//     if(e->optype == STRUCT_O)
//     {
//       decl *ct1 = getexprtype(e->args[0], scope, sn); // struct/union
//       // TODO
//       assert(!"not done yet");
//     }
    
//     if(e->optype == PSTRUCT_O)
//     {
//       decl *ct1 = getexprtype(e->args[0], scope, sn); // struct/union
//       // TODO
//       assert(!"not done yet");
//     }

//     if(e->optype == POSTINC_O)
//     {
//       decl *ct1 = getexprtype(e->args[0], scope, sn);
//       // TODO based on additive ops and assignment. must be lvalue
//       assert(!"not done yet");
//     }

//     if(e->optype == POSTDEC_O)
//     {
//       // TODO
//       assert(!"not done yet");
//     }
//   }

//   if(e->type == UNAR_E)
//   {
//     if(e->optype == PREINC_O)
//     {
//       assert(!"not done yet");
//     }

//     if(e->optype == PREDEC_O)
//     {
//       assert(!"not done yet");
//     }

//     if(e->optype == ADDR_O)
//     {
//       decl *ct1 = getexprtype(e->args[0], scope, sn);
//       assert(ct1->storespec != K_REGISTER); // no taking address of register
//       assert(ct1->lval); // can only take address of lvalue

//       decl *ct = malloc(sizeof(decl));
//       memcpy(ct, ct1, sizeof(decl));

//       // add TM_PTR to typemods
//       ct->typemods = makelist(sizeof(typemod));
//       ct->lval = 0; // no longer lvalue
//       typemod tm;
//       tm.gen.type == TM_PTR;
//       tm.ptr.isconst = tm.ptr.isvolatile = 0;
//       append(ct->typemods, &tm);

//       // write the rest of the type in
//       typemod *ct1tms = (typemod *) ct1->typemods->cont;
//       for(int i = 0; i < ct1->typemods->n; i++)
//       {
//         append(ct->typemods, ct1tm + i);
//       }

//       return ct;
//     }

//     if(e->optype == POINT_O)
//     {
//       decl *ct1 = getexprtype(e->args[0], scope, sn);
//       // check pointer
//       typemod *tm = gettm(ct1, 0);
//       assert(tm->gen.type == TM_PTR);


//       decl *ct = malloc(sizeof(decl));
//       memcpy(ct, ct1, sizeof(decl));
//       shift_front(ct->typemods);

//       // check if lval
//       // TODO struct and union also lval
//       if(isarith(ct))
//         ct->lval = 1;

//       tm = gettm(ct->typemods, 0);
//       if(tm && tm->gen.type == TM_PTR)
//         ct->lval = 1;
//     }
//   }
  
//   // TODO TYPENAME, ARGLIST
// }

// simply calls getexprtype and finds the size of the returned decl
// int sizeofexpr(expr *e)
// {
//   if(eistype(e, PRIM_E))
//   {
    
//   }
// }

