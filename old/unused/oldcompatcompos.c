// make composite types
// writes into t1
// assumes that iscompat(t1, t2) returns 1
// void makecomposite(decl *t1, decl *t2)
// {
//   int iscompat(decl *t1, decl *t2, int top, int asgn);
  
//   assert(iscompat(t1, t2, 0, 0));

//   // dattype, typequals must all be equal
//   // top-level storespecs are not supported
//   // multiple declarations aren't allowed in functions, only top-level

//   // for init and fundef, one decl may have it NULL
//   if(!t1->init)
//     t1->init = t2->init;
//   if(!t1->fundef)
//     t1->fundef = t2->fundef;

//   // now go through typemods and update any incomplete types
//   typemod *tms1 = (typemod *) t1->typemods->cont;
//   typemod *tms2 = (typemod *) t2->typemods->cont;
//   for(int i = 0; i < t1->typemods->n - 1; i++)
//   {
//     int type = tms1->gen.type;
    
//     // pointers should be identical
//     // ident should not be appearing at this stage
//     // none is avoided by n - 1 above
    
//     if(type == TM_FUNC) // incomplete function type
//     {
//       if(tms1->func.params == NULL)
//       {
//         tms1->func.params = tms2->func.params;
//       }

//       else if(tms2->func.params) // both have params, then recursively makecomposite each param
//       {
//         decl *pl1 = (decl *) tms1->func.params->cont;
//         decl *pl2 = (decl *) tms2->func.params->cont;

//         assert(iscompat(pl1, pl2, 0, 0));
        
//         for(int i = 0; i < tms1->func.params->n; i++)
//         {
//           makecomposite(pl1, pl2);
//         }
//       }
//     }

//     else if(type == TM_ARR)
//     {
//       if(tms1->arr.len == -1)
//       {
//         tms1->arr.len = tms2->arr.len;
//       }
//     }
//   }
// }


// checks if two types are equivalent
/*
   in case asgn is 1, then t1 type qualifiers may be more strict than t2
   in case top, then the current (toplevel) qualifiers for t1 and t2 may be anything, as we always pass/assign by value
*/
// int iscompat(decl *t1, decl *t2, int top, int asgn)
// {
//   // TODO equivalence for structs, unions, enums
//   // TODO qualifiers
//   // TODO typedefs?

//   // equivalent type specifier lists?
//   // if(t1->dattype != t2->dattype)
//   //   return 0;
//   // so that top functions properly we check dattype at the very end

  

//   // equivalent typedefs?

//   // we assume typedefs have all been expanded already

//   typemod *tms1 = (typemod *) t1->typemods->cont;
//   typemod *tms2 = (typemod *) t2->typemods->cont;
  
//   // // make both typemod lists abstract
//   // if(tms1[0].gen.type == TM_IDENT)
//   //   tms1++;
//   // if(tms2[0].gen.type == TM_IDENT)
//   //   tms2++;
  
//   for(;; tms1++, tms2++)
//   {
//     typemod tm1 = *tms1;
//     typemod tm2 = *tms2;

//     // same typemod type?
//     if(tm1.gen.type != tm2.gen.type)
//     {
//       return 0;
//     }

//     // based on the typemod type, check equality in different ways
//     int type = tm1.gen.type;

//     // if pointer, check qualifiers
//     if(type == TM_PTR)
//     {
//       if(top) // top level, ignore qualifiers
//       {
//         top = 0; // no longer top level
//       }
      
//       else if(asgn) // assignment, t1 quals may be superset of t2
//       {
//         if(!tm1.ptr.isconst && tm2.ptr.isconst)
//           return 0;
//         if(!tm1.ptr.isvolatile && tm2.ptr.isvolatile)
//           return 0;
//       }

//       else // else, strict equivalence in quals required (usually for multiple compatible declarations
//       {
//         if(tm1.ptr.isconst != tm2.ptr.isconst)
//           return 0;
//         if(tm2.ptr.isvolatile != tm2.ptr.isvolatile)
//           return 0;
//       }
//     }

//     if(type == TM_FUNC)
//     {
//       list *p1 = tm1.func.params;
//       list *p2 = tm2.func.params;
//       // if one or the other is unspecified, then they are equivalent
//       if(p1 && p2)
//       {
//         if(p1->n != p2->n) // same length?
//           return 0;

//         decl *pl1 = (decl *) p1->cont;
//         decl *pl2 = (decl *) p2->cont;

//         // recursively check parameters for equivalence
//         for(int i = 0; i < p1->n; i++)
//         {
//           if(!iscompat(pl1+i, pl2+i, 0, 0))
//             return 0;
//         }
//       }
//     }

//     else if(type == TM_ARR)
//     {
//       if((tm1.arr.len != -1) && (tm2.arr.len != -1))
//       {
//         if(tm1.arr.len != tm2.arr.len) // must be same length
//         {
//           return 0;
//         }
//       }
//     }

//     else if(type == TM_NONE) // reached the end
//     {
//       break;
//     }
//   }

//   if(t1->dattype != t2->dattype) // equivalent type specs?
//     return 0;

//   if(top) // toplevel -> don't care about quals
//     return 1;

//   if(asgn) // asgn -> t1 quals superset of t2
//   {
//     if(issub(t2->typequals, t1->typequals))
//     {
//       return 1;
//     }
//   }

//   // otherwise strict equality of qualifiers required
//   return seteq(t2->typequals, t1->typequals);
// }


// the lowest level of sizeof. we have a type, and we run through the typemods/dattype to get its size. called by sizeofexpr()
// int sizeoftype(ctype ct)
// {
//   int helpsizeoftype(int dt, typemod *tms);
//   int dtsize(int dt);

//   if(ct->gen.type == TM_DAT) // no typemods
//     return dtsize(ct->dat.dt);
  

//   // wrapper that separates ct into relevant components
//   // typemod *tms = ct->tms;
//   // int dattype = ct->dattype;
//   return helpsizeoftype(dattype, tms);
// }

