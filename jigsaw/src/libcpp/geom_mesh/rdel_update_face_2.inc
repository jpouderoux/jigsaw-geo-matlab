
    /*
    --------------------------------------------------------
     * RDEL-UPDATE-2: update restricted delaunay in R^2. 
    --------------------------------------------------------
     *
     * This program may be freely redistributed under the 
     * condition that the copyright notices (including this 
     * entire header) are not removed, and no compensation 
     * is received through use of the software.  Private, 
     * research, and institutional use is free.  You may 
     * distribute modified versions of this code UNDER THE 
     * CONDITION THAT THIS CODE AND ANY MODIFICATIONS MADE 
     * TO IT IN THE SAME FILE REMAIN UNDER COPYRIGHT OF THE 
     * ORIGINAL AUTHOR, BOTH SOURCE AND OBJECT CODE ARE 
     * MADE FREELY AVAILABLE WITHOUT CHARGE, AND CLEAR 
     * NOTICE IS GIVEN OF THE MODIFICATIONS.  Distribution 
     * of this code as part of a commercial system is 
     * permissible ONLY BY DIRECT ARRANGEMENT WITH THE 
     * AUTHOR.  (If you are not directly supplying this 
     * code to a customer, and you are instead telling them 
     * how they can obtain it for free, then you are not 
     * required to make any arrangement with me.) 
     *
     * Disclaimer:  Neither I nor: Columbia University, The
     * Massachusetts Institute of Technology, The 
     * University of Sydney, nor The National Aeronautics
     * and Space Administration warrant this code in any 
     * way whatsoever.  This code is provided "as-is" to be 
     * used at your own risk.
     *
    --------------------------------------------------------
     *
     * Last updated: 06 September, 2017
     *
     * Copyright 2013-2017
     * Darren Engwirda
     * de2363@columbia.edu
     * https://github.com/dengwirda/
     *
    --------------------------------------------------------
     */
     
    // from rdel_mesh_2.hpp
     
    
    /*
    --------------------------------------------------------
     * _POP-EDGE: delete edge from restricted-tria. 
    --------------------------------------------------------
     */
     
    __static_call 
    __normal_call void_type _pop_edge (
        mesh_type &_mesh ,
        iptr_type  _tpos
        )
    {
        for(auto _fpos =+3; _fpos-- != +0; )
        {
    /*--------------------------------- get edge indexing */
            iptr_type _tnod[ +3] ;
            mesh_type::tria_type::
                tria_type::
            face_node(_tnod, _fpos, +2, +1);
            _tnod[0] = _mesh._tria.
                tria(_tpos)->node(_tnod[0]);
            _tnod[1] = _mesh._tria.
                tria(_tpos)->node(_tnod[1]);

            algorithms::isort (
                &_tnod[0], &_tnod[2], 
                    std::less<iptr_type>());

            edge_data _edat, _same ;
            _edat._node[0] = _tnod[0];
            _edat._node[1] = _tnod[1];

    /*--------------------------------- remove if present */
            _mesh._pop_edge(_edat, _same)  ;
        }
    }
  
    /*
    --------------------------------------------------------
     * _POP-TRIA: delete tria from restricted-tria. 
    --------------------------------------------------------
     */
    
    __static_call
    __normal_call void_type _pop_tria (
        mesh_type &_mesh ,
        iptr_type  _tpos
        )
    {
    /*--------------------------------- get tria indexing */
        tria_data _tdat, _same;
        _tdat._node[0] = _mesh.
            _tria.tria(_tpos)->node(0);
        _tdat._node[1] = _mesh.
            _tria.tria(_tpos)->node(1);
        _tdat._node[2] = _mesh.
            _tria.tria(_tpos)->node(2);
        
    /*--------------------------------- remove if present */
        _mesh._pop_tria(_tdat, _same) ;
    }
    
    /*
    --------------------------------------------------------
     * PUSH-NBAL: add new ball to restricted-tria.
    --------------------------------------------------------
     */
    
    __static_call
    __normal_call void_type push_nbal (
        mesh_type &_mesh,
        geom_type &_geom,
        size_type &_size,
        iptr_type  _npos,
        nbal_list &_nbal,
        iptr_type  _pass,
        rdel_opts &_opts
        )
    {
        if (_mesh._tria.
             node(_npos)->feat()==hard_feat)
        {
    /*---------- push protecting ball for "hard" features */
            nbal_data _ball ;
            _ball._node[0] = _npos;
            
            _ball._ball[0] = _mesh.
                _tria.node(_npos)->pval(0) ;
            _ball._ball[1] = _mesh.
                _tria.node(_npos)->pval(1) ;
            
            real_type _rbal = _size.eval (
               &_mesh._tria.
            node(_npos)->pval(0) ,
                _mesh._tria.
            node(_npos)->idxh()) ;
            
            _ball._ball[2] = _rbal * _rbal ;
    
            _nbal.push_tail (_ball) ;        
        }
    }
    
    /*
    --------------------------------------------------------
     * PUSH-EDGE: add new edge to restricted-tria.
    --------------------------------------------------------
     */
    
    __static_call
    __normal_call void_type push_edge (
        mesh_type &_mesh ,
        geom_type &_geom ,
        size_type &_size ,
        iptr_type  _tpos ,
        edat_list &_eset ,
        escr_list &_escr ,
        typename 
    mesh_type::edge_list & _edge_test ,
        iptr_type &_nedg ,
        iptr_type &_ndup ,
        iptr_type  _pass ,
        rdel_opts &_opts
        )
    {
    /*-------------------------------- correct node dims? */       
        iptr_type _fdim =+0 ;
        for(iptr_type _node =+3; _node-- != +0; )
        {
        if (_mesh._tria.node (
            _mesh._tria.tria (
            _tpos)->node(_node))->fdim() <= +1)
            _fdim += +1 ;
        }
    /*-------------------------------- quick break if not */
        if (_fdim  < +2 ) return ;

    /*-------------------------------- check "restricted" */
        for(iptr_type _fpos =+3; _fpos-- != +0; )
        {
        /*---------------------------- extract face nodes */
            iptr_type _tnod[ +3] ;
            mesh_type::tria_type::
                tria_type::
            face_node(_tnod, _fpos, +2, +1);
            _tnod[0] = _mesh._tria.
                tria(_tpos)->node(_tnod[0]);
            _tnod[1] = _mesh._tria.
                tria(_tpos)->node(_tnod[1]);
            
        /*--------------- face contains higher dim. nodes */
            if (_mesh._tria.
                 node(_tnod[0])->fdim()>1 ||
                _mesh._tria.
                 node(_tnod[1])->fdim()>1 )
                continue   ;

            algorithms::isort (
                &_tnod[0], &_tnod[2], 
                    std::less<iptr_type>()) ;

            edge_data _fdat;
            _fdat._node[0] = _tnod[0] ;
            _fdat._node[1] = _tnod[1] ;

            edge_cost _cdat;
            _cdat._node[0] = _tnod[0] ;
            _cdat._node[1] = _tnod[1] ;

            typename mesh_type::
                     edge_list::
                item_type *_mptr = nullptr;
            if (_edge_test.find(_fdat, _mptr)) 
            {
        /*--------------------------- count bnd. repeats! */
                _ndup += 
                _mptr->_data._dups;
            
        /*--------------------------- don't test repeats! */
                continue   ;
            }

            _cdat._pass    = _pass;
            _fdat._pass    = _pass;

            _fdat._tadj    = _tpos;
            _fdat._eadj    = _fpos;
            _fdat._dups    = +0; // count num. dup's
                                 // only in hash-set
            
        /*--------------------------- call face predicate */
            char_type _feat, _topo;
            real_type _fbal[ 3];
            real_type _sbal[ 3];
            mesh_pred::edge_cost (
                _geom,_size, 
                _mesh, 
                _fdat._tadj,
                _fdat._eadj,
                _opts,_cdat,
                _fdat._part,
                _feat,_topo,
                _fdat._kind,
                _fbal,_sbal)   ;
                
        /*--------------------------- push edge onto mesh */
            if (_fdat._kind == mesh::ring_item)
                _escr. push_tail(_cdat);

            if (_fdat._kind != mesh::null_item)
                _nedg += +1 ;

            if (_fdat._kind != mesh::null_item)
                _fdat.
                _dups  = +1 ;

            if (_fdat._kind != mesh::null_item)
                _eset. push_tail(_fdat);


            _edge_test.push (_fdat) ;

        } // for(iptr_type _fpos = +3 ; _fpos-- != +0 ; )
    }
    
    /*
    --------------------------------------------------------
     * PUSH-TRIA: add new tria to restricted-tria.
    --------------------------------------------------------
     */
    
    __static_call
    __normal_call void_type push_tria (
        mesh_type &_mesh ,
        geom_type &_geom ,
        size_type &_size ,
        iptr_type  _tpos ,
        iptr_type &_sign ,
        tdat_list &_tset ,
        tscr_list &_tscr ,
        typename 
    mesh_type::tria_list & _tria_test ,
        iptr_type &_ntri ,
        iptr_type  _pass ,
        rdel_opts &_opts
        )
    {
    /*-------------------------------- check "restricted" */
        {
            tria_data _tdat;
            _tdat._node[0] = 
            _mesh._tria.tria(_tpos)->node(0) ;
            _tdat._node[1] = 
            _mesh._tria.tria(_tpos)->node(1) ;
            _tdat._node[2] = 
            _mesh._tria.tria(_tpos)->node(2) ;

            _tdat._tadj    = _tpos;

        /*--------------- face contains higher dim. nodes */
            if (_mesh._tria.node(
                _tdat._node[0])->fdim() > 2 ||
                _mesh._tria.node(
                _tdat._node[1])->fdim() > 2 ||
                _mesh._tria.node(
                _tdat._node[2])->fdim() > 2 )
                return ;

            typename mesh_type::
                     tria_list::
                     item_type *_mptr=nullptr;
            if (_tria_test.find(_tdat, _mptr))
            { 
        /*--------------------------- don't test repeats! */
                return ;
            }

            _tria_test.push( _tdat) ;

        /*--------------------------- calc tria cost/kind */
            tria_cost _cdat;
            _cdat._node[0] = _tdat._node[0] ;
            _cdat._node[1] = _tdat._node[1] ;
            _cdat._node[2] = _tdat._node[2] ;

        /*--------------------------- call tria predicate */
            _tdat._part =  _sign ;

            _cdat._pass =  _pass ;
            _tdat._pass =  _pass ;

            mesh_pred::tria_cost (
                _geom,_size, 
                _mesh, 
                _tdat._tadj,
                _opts,_cdat,
                _tdat._part,
                _tdat._kind)   ;

            _sign = _tdat. _part ;

        /*--------------------------- push edge onto mesh */
            if (_tdat._kind == mesh::ring_item)
                _tscr. push_tail(_cdat);

            if (_tdat._kind != mesh::null_item)
                _ntri += +1 ;

            if (_tdat._kind != mesh::null_item)
                _tset. push_tail(_tdat);
        }
    }

    /*
    --------------------------------------------------------
     * TRIA-CIRC: calc. circumball for tria.
    --------------------------------------------------------
     */

    __static_call
    __normal_call void_type tria_circ ( // get "exact" tbal?
        mesh_type &_mesh ,
        iptr_type  _tpos
        )
    {
        iptr_type _fnod = +0;
        for (auto _inod = +3; _inod-- != +0; )
        {
        if (_mesh._tria.node(
            _mesh._tria.tria(
            _tpos)->node(_inod))->fdim() < +2)
            _fnod += +1 ;
        }
    
        if (_fnod <= +1 )
        {
    /*---------------------- calc. ball in floating-point */
            real_type _tbal[3] ;
            geometry::tria_ball_2d (
                _tbal , 
           &_mesh._tria.node(
            _mesh._tria.tria(
            _tpos)->node(0))->pval(0) , 
           &_mesh._tria.node(
            _mesh._tria.tria(
            _tpos)->node(1))->pval(0) ,
           &_mesh._tria.node(
            _mesh._tria.tria(
            _tpos)->node(2))->pval(0)
                ) ;
                
            _mesh._tria.tria(
            _tpos)->circ(0)= _tbal[0] ;
            _mesh._tria.tria(
            _tpos)->circ(1)= _tbal[1] ;
        }
        else
        {
    /*---------------------- calc. ball in adaptive-prec. */
            dd_flt _pos1[ +2] = {
            _mesh._tria.node(
            _mesh._tria.tria(
            _tpos)->node(0))->pval(0) ,
            _mesh._tria.node(
            _mesh._tria.tria(
            _tpos)->node(0))->pval(1)
                } ;
            dd_flt _pos2[ +2] = {
            _mesh._tria.node(
            _mesh._tria.tria(
            _tpos)->node(1))->pval(0) ,
            _mesh._tria.node(
            _mesh._tria.tria(
            _tpos)->node(1))->pval(1)
                } ;
            dd_flt _pos3[ +2] = {
            _mesh._tria.node(
            _mesh._tria.tria(
            _tpos)->node(2))->pval(0) ,
            _mesh._tria.node(
            _mesh._tria.tria(
            _tpos)->node(2))->pval(1)
                } ;
            
            dd_flt _tbal[ +3] ;
            geometry::tria_ball_2d (
                _tbal, _pos1, 
                _pos2, _pos3) ;
           
            _mesh._tria.tria(
            _tpos)->circ(0)= _tbal[0] ;
            _mesh._tria.tria(
            _tpos)->circ(1)= _tbal[1] ;
        }
    }

    /*
    --------------------------------------------------------
     * PUSH-RDEL: push faces onto restricted tria.
    --------------------------------------------------------
     */

    __static_call
    __normal_call void_type push_rdel (
        geom_type &_geom ,
        size_type &_size ,
        mesh_type &_mesh ,
        iptr_list &_nnew ,
        iptr_list &_tnew ,
        escr_list &_escr ,
        edat_list &_edat ,
        tscr_list &_tscr ,
        tdat_list &_tdat ,
        nbal_list &_nbal ,
        iptr_type  _sign ,
        iptr_type  _pass ,
        mode_type  _dim0 ,
        mode_type  _dim1 ,
        rdel_opts &_opts
        )
    {
    /*------------------------- init. for local hash obj. */
        typename 
            mesh_type::edge_list _eset (
        typename mesh_type::edge_hash(),
        typename mesh_type::edge_pred(), 
           +.8, _mesh._eset.get_alloc()) ;
        typename 
            mesh_type::tria_list _tset (
        typename mesh_type::tria_hash(),
        typename mesh_type::tria_pred(), 
           +.8, _mesh._tset.get_alloc()) ;

    /*------------------------- push alloc. for hash obj. */
        _eset._lptr.set_count (
            _tnew.count() * +3 , 
        containers::loose_alloc, nullptr);
        _tset._lptr.set_count (
            _tnew.count() * +1 , 
        containers::loose_alloc, nullptr);

    /*------------------------- no. "restricted" subfaces */
        iptr_type _nedg = +0 ;
        iptr_type _ntri = +0 ;
        
        iptr_type _ndup = +0 ;

    /*------------------------- flag if we're not testing */
        bool_type _safe ;
       _safe = (_dim0>=2) ? false : true ;

    /*------------------------- calc. voronoi-dual points */
        for( auto _iter  = _tnew.head(); 
                  _iter != _tnew.tend(); 
                ++_iter  )
        {
            tria_circ(_mesh,*_iter) ;
        }
    
    /*------------- push any new protecting balls created */
        if (_dim0 <= node_mode &&
            _dim1 >= node_mode )
        {
        for( auto _iter  = _nnew.head(); 
                  _iter != _nnew.tend(); 
                ++_iter  )
        {     
            push_nbal(_mesh, _geom ,
                      _size,*_iter , 
                      _nbal, 
                      _pass, _opts) ;
        } 
        }
    /*------------- push any new restricted edges created */
        if (_dim0 <= edge_mode &&
            _dim1 >= edge_mode )
        {
        for( auto _iter  = _tnew.head(); 
                  _iter != _tnew.tend(); 
                ++_iter  )
        {        
            push_edge(_mesh, _geom ,
                      _size,*_iter , 
                      _edat, _escr , 
                      _eset, _nedg , 
                      _ndup, 
                      _pass, _opts) ;
        } 
        }
    /*------------- push any new restricted trias created */
        if (_dim0 <= tria_mode && 
            _dim1 >= tria_mode )
        {
        
      //if (_nedg >= +1) _safe = false ;
        if (_ndup >= +1) _safe = false ;
        
    /*-------------------------- compute cavity tria cost */
        for( auto _iter  = _tnew.head(); 
                  _iter != _tnew.tend(); 
                ++_iter )
        {
           _sign =  (!_safe) ? -1 : _sign ;

            push_tria(_mesh, _geom ,
                      _size,*_iter , 
                      _sign, _tdat , 
                      _tscr, _tset , 
                      _ntri,
                      _pass, _opts) ;
        }
        }
    }
    
    
    