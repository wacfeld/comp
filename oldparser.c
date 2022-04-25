

// evaluate primary expressions
// this also parses top-level type names because we have to do that somewhere
link *parseprimexpr(link *chain)
{
  // chain is a linked list of tokens and expressions
  // ultimately the goal is to turn everything into a single expression
  link* curl = chain;

  while(curl != NULL)
  {
    // printf("%p\n", curl);
    // (expression)
    if(lisatom(curl, PARENOP))
    {
      if(lisatom(curl->right, PARENCL)) // empty parens -> funcall, ignore for now, deal with later as special case
      {
        curl = curl->right->right;
        continue;
      }
        
      // putd(0);
      // find matching
      // int parendep = 1;
      // link *parr = curl->right; // paren right
      // while(parendep > 0) // find opposite
      // {
      //   if(lisatom(parr, PARENOP)) parendep++;
      //   if(lisatom(parr, PARENCL)) parendep--;
      //   assert(parendep >= 0 && parr != NULL);
      //   parr = parr->right;
      // }
      link *parr = findmatch(curl, RIGHT, PARENOP, PARENCL);

      // parr = parr->left; // move onto )

      // detach
      curl->right->left = NULL;
      parr->left->right = NULL;

      if(isdeclspec(*curl->cont.tok)) // it's a cast, therefore not a primary expression
      {
        // convert to token list
        token *abstype = ll2tokl(curl->right);
        // parse type
        ctype *ct = parsedecl(abstype, 1);
        
        // put into expression
        expr *newe = calloc(1,sizeof(expr));
        // newe->type = TYPENAME;
        etypeadd(newe, TYPENAME);
        // no optype, or other things, only ct
        newe->ct = ct;

        // remove the parens. type-name can only ever occur within exactly one pair of parens, so this simplifies things
        // reattach
        attach(curl->left, newe);
        attach(newe, parr->right);
        curl = newe; // recentre

        // curl = opl; // move over
        continue;
      }

      // otherwise it's an expression or a function call
      // either way, it's a comma-separated list of assignment expressions, possibly empty if function call

      // disconnect
      // link *left = curl->left->left; // just outside the parens
      // link *right = opl->right->right;
      // curl->left = NULL;
      // opl->right = NULL;

      // evaluate
      link *l = parseexpr(curl->right);
      // encapsulate it in a primary expression (this is important!)
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,PRIM_E);
      newe->optype = PAREN_O;
      newe->args = calloc(1,sizeof(expr));
      newe->args[0] = l->cont.exp;
      l->cont.exp = newe;

      // write
      // link *l = malloc(sizeof(link));
      // l->type = EXPR_L;
      // l->cont.exp = exp;

      // reconnect
      attach(curl->left, l);
      attach(l, parr->right);
      // l->left = left;
      // l->right = right;
      // if(left) left->right = l;
      // if(right) right->left = l;
      // TODO free all those tokens, either inside the recursive call, or here
      // solution: free all at the end, from the translation unit. let them linger until then

      // reposition
      curl = l;
    }

    // constants
    else if(listok(curl, INTEGER)) // integer constant
    {
    // putd(1);
      expr *e = calloc(1,sizeof(expr));
      etypeadd(e,PRIM_E);
      e->optype = INT_O;

      e->tok = curl->cont.tok;

      curl->type = EXPR_L;
      curl->cont.exp = e;
    }
    else if(listok(curl, CHAR))
    {
// putd(2);
      expr *e = calloc(1,sizeof(expr));
      etypeadd(e,PRIM_E);
      e->optype = CHAR_O;

      e->tok = curl->cont.tok;

      curl->type = EXPR_L;
      curl->cont.exp = e;
    }
    else if(listok(curl, FLOATING))
    {
// putd(3);
      expr *e = calloc(1,sizeof(expr));
      etypeadd(e,PRIM_E);
      e->optype = FLOAT_O;
      e->tok = curl->cont.tok;

      curl->type = EXPR_L;
      curl->cont.exp = e;
    }

    else if(listok(curl, STRLIT)) // string literal
    {
    // putd(4);
      expr *e = calloc(1,sizeof(expr));
      etypeadd(e,PRIM_E);
      e->optype = STRING_O;
      e->tok = curl->cont.tok;

      curl->type = EXPR_L;
      curl->cont.exp = e;
    }

    else if(listok(curl, IDENT)) // identifiers
    {
    // putd(5);
      expr *e = calloc(1,sizeof(expr));
      etypeadd(e,PRIM_E);
      e->optype = IDENT_O;
      e->tok = curl->cont.tok;

      curl->type = EXPR_L;
      curl->cont.exp = e;
    }
    // otherwise it's not a primary expression

    // else putd(6);
    // move on to next link
    if(curl->right == NULL) break; // break preemptively, therefore this while loop could be a while(1) but anyway, it's so we don't get left with NULL
    curl = curl->right;
  }

  // find the leftmost link again, to return
  // due to the recursive (expression) parsing, chain may no longer point to it
  while(curl->left != NULL)
  {
    // printf("%p\n", curl);
    curl = curl->left;
  }
  return curl;
}


// postfix expression
link *parsepostexpr(link *chain)
{
  // parse primary expressions
  chain = parseprimexpr(chain);
  // promote all primary expressions to postfix
  link *curl = chain;
  while(curl != NULL)
  {
    if(leistype(curl, PRIM_E))
    {
      etypeadd(curl->cont.exp,POST_E);
    }
    curl = curl->right;
  }
  
  curl = chain; // back to start
  do // fold in post expressions until none left
  {
    // puts("----");
    // putll(curl);
    if(!leistype(curl, POST_E)) // if not postfix expression, pass over
    {
      curl = curl->right;
      continue;
    }

    if(lisatom(curl->right, BRACKOP)) // postfix-expression[expression]
    {
      // link* indr = curl->right; // index right
      link *indl = curl->right->right; // [

      // int brackdep = 1; // find matching

//       while(brackdep)
//       {
//         indr = indr->right;
//         if(lisatom(indr, BRACKOP)) brackdep++;
//         if(lisatom(indr, BRACKCL)) brackdep--;
//         assert(brackdep >= 0 && indr != NULL);
//       }
//       indr = indr->left; // left of ]
      link *indr = findmatch(curl->right, RIGHT, BRACKOP, BRACKCL);
      indr = indr->left; // left of ]

      link *outr = indr->right->right; // right of ]
      // detach
      indr->right = NULL;
      indl->left = NULL;

      // parse index expression
      indl = parseexpr(indl);
      expr *e = indl->cont.exp; // extract expression from link

      // make new expression from base and index
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,POST_E); // postfix
      newe->optype = ARR_O; // array indexing
      newe->args = malloc(sizeof(expr) * 2); // 2 args
      newe->args[0] = curl->cont.exp; // base
      newe->args[1] = e; // index

      // replace with new expression
      curl->cont.exp = newe;
      // reattach
      // curl->right = outr;
      // outr->left = curl;
      attach(curl, outr);
      
      // curl = chain; // restart since everything's been modified
      // we can probably not restart from the beginning, due to how postfix works, but this also works fine
    }

    else if(lisop(curl->right, PAREN_O)) // function call time
    {
      // TODO
    }

    else if(lisatom(curl->right, PARENOP)) // postfix-expression(argument-expression-list_opt)
    {
      // parens are a special case
      // paren tokens can only be (casts) and empty() now

      if(isdeclspec(*curl->cont.tok)) // cast, move over
      {
        // int parendep = 1;
        // curl = curl->right;
        // while(parendep)
        // {
        //   if(lisatom(curl), PARENOP) parendep++;
        //   if(lisatom(curl), PARENCL) parendep--;
        //   assert(parendep >= 0 && curl != NULL);
        //   curl = curl->right;
        // }

        curl = findmatch(curl, RIGHT, PARENOP, PARENCL)->right;
        continue;
      }

      if(lisatom(curl->right, PARENCL)) // empty args
      {
        // TODO
        curl = curl->right->right;
        continue;
      }

      // link *argl = curl->right->right; // left of arguments
      // link *argr = curl->right->right;
      
      // expr *newe = malloc(sizeof(expr));
      // newe->type = POST_E;
      // newe->optype = FUN_O;
      // newe->arglen = 0;
      // int size = 5;
      // newe->args = malloc(sizeof(expr) * size);

      // newe->args[0] = curl->cont.exp; // function name is the first thing
      // newe->arglen++;

      // int parendep = 1;
      // while(parendep) // while in argument parentheses
      // {
      //   if(lisatom(argr, PARENOP)) parendep++;
      //   if(lisatom(argr, PARENCL)) parendep--;
      //   assert(parendep >= 0 && argr != NULL);

      //   if((lisatom(argr, COMMA) && parendep == 1) || parendep == 0) // end of argument, evaluate
      //   {
      //     // detach
      //     argr->left->right = NULL;
      //     argl->left = NULL;

      //     // parse assignment expression
      //     argl = parseasgnexpr(argl);
      //     expr *e = argl->cont.exp;

      //     // append to argument list
      //     resize(newe->args, size, newe->arglen);
      //     newe->args[newe->arglen] = e;
      //     newe->arglen++;
          
      //     // setup for next argument
      //     argl = argr->right;
      //     argr = argr->right;
      //   }

      //   else
      //     argr = argr->right;

      // }

      // wrap up, reattach
      // curl->cont.exp = newe;
      // attach(curl, argr);
      // // curl->right = argr;
      // // argr->left = curl;
      
      // curl = chain; // restart
    }

    else if(lisatom(curl->right, DOT))
    {
      assert(listok(curl->right->right, IDENT)); // struct members are always single identifiers

      // create new expression
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,POST_E);
      newe->optype = STRUCT_O;
      newe->args = malloc(sizeof(expr) * 2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      // attach
      curl->cont.exp = newe;
      // link *temp = curl->right->right->right;
      attach(curl, curl->right->right->right);
      // curl->right = curl->right->right->right;
      // curl->right->left = curl;

      // go back to beginning
      // curl = chain;
    }

    else if(lisatom(curl->right, ARROW))
    {
      assert(lisop(curl->right->right, IDENT_O));
      
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,POST_E);
      newe->optype = PSTRUCT_O;
      newe->args = malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;
      // link *temp = curl->right->right->right;
      attach(curl, curl->right->right->right);

      // curl = chain;
    }

    else if(lisatom(curl->right, INC))
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,POST_E);
      newe->optype = POSTINC_O;
      newe->args = malloc(sizeof(expr));
      newe->args[0] = curl->cont.exp;

      curl->cont.exp = newe;
      // link *temp = curl->right->right;
      attach(curl, curl->right->right);

      // curl = chain;
    }

    else if(lisatom(curl->right, DEC))
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,POST_E);
      newe->optype = POSTDEC_O;
      newe->args = malloc(sizeof(expr));
      newe->args[0] = curl->cont.exp;

      curl->cont.exp = newe;
      // link *temp = curl->right->right;
      attach(curl, curl->right->right);

      // curl = chain;
    }

    else
    {
      if(curl->right == NULL) break; // break preemptively
      curl = curl->right; // move on
    }
  } while(curl != NULL);


  // find start again
  while(curl->left != NULL)
  {
    // printf("%p\n", curl);
    curl = curl->left;
  }
  return curl;
  
}

// unary and cast expressions mutually reference each other so they have to be parsed together
link *parsecastunaryexpr(link *chain)
{
  // parse post
  chain = parsepostexpr(chain);

  int modified;
  link *curl = chain;

  do // we go back and forth between unary and cast, until modifications stop being made
  {
    modified = 0;

    // promote post to unary
    // go to left
    // while(curl->left != NULL)
    // {
    //   curl = curl->left;
    // }
    leftend(curl);

    link *temp = curl;
    while(temp != NULL)
    {
      if(leistype(temp, POST_E))
      {
        etypeadd(temp->cont.exp,UNAR_E);
      }
      temp = temp->right;
    }

    // navigate to end, because we're parsing right to left
    // while(curl->right != NULL)
    // {
    //   curl = curl->right;
    // }
    rightend(curl);

    // unary expressions
    do
    {
      // if(!leistype(curl, UNAR_E)) // if not unary expression, skip
      // {
      //   curl = curl->left;
      //   continue;
      // }

      if(leistype(curl, UNAR_E) && lisatom(curl->left, INC)) // ++ unary-expr
      {
        expr *newe = calloc(1,sizeof(expr));
        etypeadd(newe,UNAR_E);
        newe->optype = PREINC_O;
        newe->args = malloc(sizeof(expr));
        newe->args[0] = curl->cont.exp;

        curl->cont.exp = newe;
        // link *temp = curl->left->left;
        attach(temp, curl->left->left);

        modified = 1;
      }

      else if(leistype(curl, UNAR_E) && lisatom(curl->left, DEC)) // -- unary-expr
      {
        expr *newe = calloc(1,sizeof(expr));
        etypeadd(newe,UNAR_E);
        newe->optype = PREDEC_O;

        newe->args = malloc(sizeof(expr));
        newe->args[0] = curl->cont.exp;

        curl->cont.exp = newe;
        // link *temp = curl->left->left;
        attach(temp, curl->left->left);

        modified = 1;
      }

      // unary-operator cast-expression
      else if(leistype(curl, CAST_E) && lisunaryop(curl->left))
      {
        // get operator
        int atom = curl->left->cont.tok.atom.type;

        // create new expression
        expr *newe = calloc(1,sizeof(expr));
        etypeadd(newe,UNAR_E);

        int optype;
        if(atom      == BITAND) optype = ADDR_O;
        else if(atom == STAR) optype   = POINT_O;
        else if(atom == PLUS) optype   = UPLUS_O;
        else if(atom == MIN) optype    = UMIN_O;
        else if(atom == BITNOT) optype = BNOT_O;
        else if(atom == LOGNOT) optype = LNOT_O;

        newe->optype = optype;
        newe->args = malloc(sizeof(expr)); // unary obviously has 1 arg
        newe->args[0] = curl->cont.exp;
        curl->cont.exp = newe;

        // reattach
        attach(curl->left->left, curl);

        // signal modified
        modified = 1;

        // we could do curl = curl->left here because it's now a UNAR_E but it's not necessary
      }

      // else if(lisatom(curl->left, PARENCL)) // cast
      // {
      //   int parendep = 1;
      //   link *opparen = curl->left;

      //   while(parendep > 0 && opparen != NULL) // find matching
      //   {
      //     if(lisatom(opparen, PARENCL)) parendep++;
      //     if(lisatom(opparen, PARENOP)) parendep--;
      //     assert(parandep >= 0);
      //     opparen = opparen->left;
      //   }

      //   link *outl = opparen;
      //   opparen = opparen->right->right; // move in

      //   // detach
      //   opparen->left = NULL;
      //   curl->left->left->right = NULL;

      //   // evaluate
      //   token *abstype = ll2tokl(opparen); // convert to token list
      //                                      // evaluate
      //   ctype *ct = parsedecl(abstype, 1); // parse only declaration

      //   // new expression
      //   expr *newe = malloc(sizeof(expr));
      //   newe->type = UNAR_E;
      //   newe->optype = CAST_O;

      //   newe->args = malloc(sizeof(expr));
      //   newe->args[0] = curl->cont.exp;
      //   newe->ct = ct; // tell it what to cast to
      //   curl->cont.exp = newe;

      //   // absorb cast
      //   attach(outl, curl);
      // }

      else if(leistype(curl, UNAR_E) && lisatom(curl->left, SIZEOF)) // sizeof unary-expression
      {
        // TODO (this is a compile-time expression, must be evaluated now or soon)
        modified = 1;
      }

      else if(leistype(curl, TYPENAME) && lisatom(curl->left, SIZEOF)) // sizeof(type-name)
      {
        // TODO (see above)
        modified = 1;
      }

      else
      {
        if(curl->left == NULL) break; // break preemptively
        curl = curl->left; // move on
      }
    } while(1);

    // because we broke preemptively curl is not NULL
    leftend(curl);
    
    // unary -> cast
    link *temp = curl;
    while(temp != NULL)
    {
      if(leistype(temp, UNAR_E))
      {
        etypeadd(temp->cont.exp,CAST_E);
      }
      temp = temp->right;
    }

    rightend(curl);

    // cast expressions
    do
    {
      if(leistype(curl, CAST_E) && leistype(curl->left, TYPENAME)) // cast
      {
        expr *newe = calloc(1,sizeof(expr));
        etypeadd(newe,CAST_E);
        newe->optype = CAST_O;
        newe->args = malloc(sizeof(expr)*2); // cast, expr
        newe->args[0] = curl->left->cont.exp; // cast
        newe->args[1] = curl->cont.exp; // expr

        curl->cont.exp = newe;

        attach(curl->left->left, curl);

        modified = 1;
      }

      else
      {
        if(curl->left == NULL) break;
        curl = curl->left;
      }
    } while(1);

  } while(modified);


  leftend(curl);
  return curl;
}

// the following functions are extremely repetitive and therefore poorly implemented

// multiplicative expression
link *parsemultexpr(link *chain)
{
  // parse cast expression
  chain = parsecastunaryexpr(chain);

  // cast -> mult
  link *curl = chain;
  link *temp = curl;
  while(temp != NULL)
  {
    if(leistype(temp, CAST_E))
    {
      etypeadd(temp->cont.exp,MULT_E);
    }
    temp = temp->right;
  }
  
  while(1)
  {
    if(!leistype(curl, MULT_E))
    {
      curl = curl->right;
      continue;
    }

    if(lisatom(curl->right, STAR) && leistype(curl->right->right, CAST_E)) // mult-expr * cast-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,MULT_E);
      newe->optype = MULT_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else if(lisatom(curl->right, DIV) && leistype(curl->right->right, CAST_E)) // mult-expr / cast-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,MULT_E);
      newe->optype = DIV_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else if(lisatom(curl->right, MOD) && leistype(curl->right->right, CAST_E)) // mult-expr % cast-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,MULT_E);
      newe->optype = MOD_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else
    {
      if(curl->right == NULL) break;
      curl = curl->right;
    }
  }

  leftend(curl);
  return curl;
}

// additive expression
link *parseaddexpr(link *chain)
{
  // parse mult expr
  chain = parsemultexpr(chain);

  // mult -> add
  link *curl = chain;
  link *temp = curl;
  while(temp != NULL)
  {
    if(leistype(temp, MULT_E))
    {
      etypeadd(temp->cont.exp,ADD_E);
    }
    temp = temp->right;
  }
  
  while(1)
  {
    if(!leistype(curl, ADD_E))
    {
      curl = curl->right;
      continue;
    }

    if(lisatom(curl->right, PLUS) && leistype(curl->right->right, MULT_E)) // add-expr + mult-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,ADD_E);
      newe->optype = ADD_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else if(lisatom(curl->right, MIN) && leistype(curl->right->right, MULT_E)) // add-expr - mult-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,ADD_E);
      newe->optype = SUB_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else
    {
      if(curl->right == NULL) break;
      curl = curl->right;
    }
  }

  leftend(curl);
  return curl;
}

link *parseshiftexpr(link *chain)
{
  // parse add expr
  chain = parseaddexpr(chain);

  // add -> shift
  link *curl = chain;
  link *temp = curl;
  while(temp != NULL)
  {
    if(leistype(temp, ADD_E))
    {
      etypeadd(temp->cont.exp,SHIFT_E);
    }
    temp = temp->right;
  }
  
  while(1)
  {
    if(!leistype(curl, SHIFT_E))
    {
      curl = curl->right;
      continue;
    }

    if(lisatom(curl->right, SHL) && leistype(curl->right->right, ADD_E)) // shift-expr << add-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,SHIFT_E);
      newe->optype = SHL_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else if(lisatom(curl->right, SHR) && leistype(curl->right->right, ADD_E)) // shift-expr >> add-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,SHIFT_E
      newe->optype = SHR_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else
    {
      if(curl->right == NULL) break;
      curl = curl->right;
    }
  }

  leftend(curl);
  return curl;
}

link *parserelexpr(link *chain)
{
  // parse shift
  chain = parseshiftexpr(chain);

  // shift -> rel
  link *curl = chain;
  link *temp = curl;
  while(temp != NULL)
  {
    if(leistype(temp, SHIFT_E))
    {
      etypeadd(temp->cont.exp,RELAT_E);
    }
    temp = temp->right;
  }
  
  while(1)
  {
    if(!leistype(curl, RELAT_E))
    {
      curl = curl->right;
      continue;
    }

    if(lisatom(curl->right, LESS) && leistype(curl->right->right, SHIFT_E)) // rel-expr < shift-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,RELAT_E);
      newe->optype = LT_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else if(lisatom(curl->right, GREAT) && leistype(curl->right->right, SHIFT_E)) // rel-expr > shift-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,RELAT_E);
      newe->optype = GT_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else if(lisatom(curl->right, LEQ) && leistype(curl->right->right, SHIFT_E)) // rel-expr <= shift-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,RELAT_E);
      newe->optype = LEQ_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else if(lisatom(curl->right, GEQ) && leistype(curl->right->right, SHIFT_E)) // rel-expr >= shift-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,RELAT_E);
      newe->optype = GEQ_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else
    {
      if(curl->right == NULL) break;
      curl = curl->right;
    }
  }

  leftend(curl);
  return curl;

}

link *parseeqexpr(link *chain)
{
  // parse rel expression
  chain = parserelexpr(chain);

  // rel -> eq
  link *curl = chain;
  link *temp = curl;
  while(temp != NULL)
  {
    if(leistype(temp, RELAT_E))
    {
      etypeadd(temp->cont.exp,EQUAL_E);
    }
    temp = temp->right;
  }
  
  while(1)
  {
    if(!leistype(curl, EQUAL_O))
    {
      curl = curl->right;
      continue;
    }

    if(lisatom(curl->right, EQEQ) && leistype(curl->right->right, RELAT_E)) // eq-expr == rel-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,EQUAL_E);
      newe->optype = EQEQ_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else if(lisatom(curl->right, NOTEQ) && leistype(curl->right->right, RELAT_E)) // eq-epr != rel-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,EQUAL_E);
      newe->optype = NEQ_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else
    {
      if(curl->right == NULL) break;
      curl = curl->right;
    }
  }

  leftend(curl);
  return curl;

}

link *parseandexpr(link *chain)
{
  // parse eq expression
  chain = parseeqexpr(chain);

  // eq -> and
  link *curl = chain;
  link *temp = curl;
  while(temp != NULL)
  {
    if(leistype(temp, EQUAL_E))
    {
      etypeadd(temp->cont.exp,AND_E);
    }
    temp = temp->right;
  }

  while(1)
  {
    if(!leistype(curl, AND_E))
    {
      curl = curl->right;
      continue;
    }

    if(lisatom(curl->right, BITAND) && leistype(curl->right->right, EQUAL_E)) // and-expr & eq-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,AND_E);
      newe->optype = BAND_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else
    {
      if(curl->right == NULL) break;
      curl = curl->right;
    }
  }

  leftend(curl);
  return curl;

}

link *parsexorexpr(link *chain)
{
  // parse and expression
  chain = parseandexpr(chain);

  // and -> xor
  link *curl = chain;
  link *temp = curl;
  while(temp != NULL)
  {
    if(leistype(temp, AND_E))
    {
      etypeadd(temp->cont.exp,XOR_E);
    }
    temp = temp->right;
  }

  while(1)
  {
    if(!leistype(curl, XOR_E))
    {
      curl = curl->right;
      continue;
    }

    if(lisatom(curl->right, XOR) && leistype(curl->right->right, AND_E)) // xor-expr ^ and-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,XOR_E);
      newe->optype = XOR_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else
    {
      if(curl->right == NULL) break;
      curl = curl->right;
    }
  }

  leftend(curl);
  return curl;

}

link *parseorexpr(link *chain)
{
  // parse xor expression
  chain = parsexorexpr(chain);

  // xor -> or
  link *curl = chain;
  link *temp = curl;
  while(temp != NULL)
  {
    if(leistype(temp, XOR_E))
    {
      etypeadd(temp->cont.exp,OR_E);
    }
    temp = temp->right;
  }

  while(1)
  {
    if(!leistype(curl, OR_E))
    {
      curl = curl->right;
      continue;
    }

    if(lisatom(curl->right, OR) && leistype(curl->right->right, XOR_E)) // or-expr | xor-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,OR_E);
      newe->optype = BOR_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else
    {
      if(curl->right == NULL) break;
      curl = curl->right;
    }
  }

  leftend(curl);
  return curl;

}

link *parselandexpr(link *chain)
{
  // parse or expression
  chain = parseorexpr(chain);

  // or -> land
  link *curl = chain;
  link *temp = curl;
  while(temp != NULL)
  {
    if(leistype(temp, OR_E))
    {
      etypeadd(temp->cont.exp,LAND_E);
    }
    temp = temp->right;
  }

  while(1)
  {
    if(!leistype(curl, LAND_E))
    {
      curl = curl->right;
      continue;
    }

    if(lisatom(curl->right, LOGAND) && leistype(curl->right->right, OR_E)) // land-expr && or-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,LAND_E);
      newe->optype = LAND_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else
    {
      if(curl->right == NULL) break;
      curl = curl->right;
    }
  }

  leftend(curl);
  return curl;


}

link *parselorexpr(link *chain)
{
  // parse land expression
  chain = parselandexpr(chain);

  // land -> lor
  link *curl = chain;
  link *temp = curl;
  while(temp != NULL)
  {
    if(leistype(temp, LAND_E))
    {
      etypeadd(temp->cont.exp,LOR_E);
    }
    temp = temp->right;
  }

  while(1)
  {
    if(!leistype(curl, LOR_E))
    {
      curl = curl->right;
      continue;
    }

    if(lisatom(curl->right, LOGOR) && leistype(curl->right->right, LAND_E)) // lor-expr || land-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,LOR_E);
      newe->optype = LOR_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else
    {
      if(curl->right == NULL) break;
      curl = curl->right;
    }
  }

  leftend(curl);
  return curl;

}

// TODO constant expressions

link *parsecondexpr(link *chain)
{
  chain = parselorexpr(chain);

  link *curl = chain;
  
  // promote
  link *temp = curl;
  while(temp != NULL)
  {
    if(leistype(temp, LOR_E))
    {
      etypeadd(temp->cont.exp, COND_E);
    }
    temp = temp->right;
  }

  rightend();
  while(1)
  {
    // lor-expr ? expr : cond-expr
    if(leistype(curl, COND_E) && lisatom(curl->left, COLON))
    {
      // find matching question for colon
      link *colon = curl->left;
      link *quest = findmatch(colon, LEFT, COLON, QUESTION);

      // make sure it's proper form
      assert(leistype(quest->left, LOR_E));

      // detach middle expression & parse
      colon->left->right = NULL;
      quest->right->left = NULL;
      link *l = parseexpr(quest->right);

      // create new
      expr *newe = calloc(1, sizeof(expr));
      etypeadd(newe, COND_E);
      newe->optype = TERN_O;
      newe->args = malloc(sizeof(expr)*3);
      newe->args[0] = quest->left->cont.exp;
      newe->args[1] = l->cont.exp;
      newe->args[2] = curl->cont.exp;
      curl->cont.exp = newe;

      // attach
      attach(quest->left->left, curl);
    }

    else
    {
      if(curl->left == NULL) break;
      curl = curl->left;
    }
  }

  leftend(curl);
  return curl;
}
