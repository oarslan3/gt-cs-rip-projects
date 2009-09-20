(define (domain sokoban-domain)
	(:requirements :equality)
	(:predicates (space ?x)       ; x is a grid square coordinate, and is movable space
							 (is-left ?x ?y)  ; x, y are grid squares, and x is to the left of y
							 (is-above ?x ?y) ; x, y are grid squares, and x is above y
							 (at ?x ?y)       ; object x is on grid square y
							 ;(clear ?x)				; grid space x is clear
							 (dude ?x)				; object x is the small robot thing
							 (block ?x)				; object x is a block
							 (has-block ?x)		; space x has a block
							 ;(has-dude ?x)		; space x has the dude
							 )

	(:action move-up; move up without pushing anything
			:parameters (?dude ?start ?end)
			:precondition (and (dude ?dude)            
 												 (is-above ?end ?start)
												 (at ?dude ?start)
												 (space ?start)
												 (space ?end)
								 				 (not (= ?start ?end))
												 (not (has-block ?end))
												 (not (has-block ?start)))
			:effect (and 
								(at ?dude ?end)
								(not (at ?dude ?start))))

	(:action move-down; move down without pushing anything
			:parameters (?dude ?start ?end)
			:precondition (and (dude ?dude)            
										(is-above ?start ?end)
										(at ?dude ?start)
										(space ?start)
										(space ?end)
										(not (= ?start ?end))
										(not (has-block ?end))
										(not (has-block ?start)))
			:effect (and 
							(at ?dude ?end)
							(not (at ?dude ?start))))

	(:action move-left; move left without pushing anything
			:parameters (?dude ?start ?end)
			:precondition (and (dude ?dude)            
										(is-left ?end ?start)
										(at ?dude ?start)
										(space ?start)
										(space ?end)
										(not (= ?start ?end))
										(not (has-block ?end))
										(not (has-block ?start)))
			:effect (and 
							(at ?dude ?end)
							(not (at ?dude ?start))))

	(:action move-right; move right without pushing anything
			:parameters (?dude ?start ?end)
			:precondition (and (dude ?dude)            
										(is-left ?start ?end)
										(at ?dude ?start)
										(space ?start)
										(space ?end)
										(not (= ?start ?end))
										(not (has-block ?end))
										(not (has-block ?start)))
			:effect (and 
							(at ?dude ?end)
							(not (at ?dude ?start))))

	(:action move-block-up
			:parameters (?dude ?block ?dude-start ?block-start ?block-end)
			:precondition (and (dude ?dude)  ; get typing correct
										(block ?block) 
										(not (= ?dude-start ?block-start))
										(not (= ?block-start ?block-end))
										(not (= ?dude-start ?block-end))
										(space ?dude-start)
										(space ?block-start)
										(space ?block-end)
										(has-block ?block-start)
										(not (has-block ?dude-start))
										(not (has-block ?block-end))
										(is-above ?block-start ?dude-start)
										(is-above ?block-end ?block-start)
										(at ?dude ?dude-start) ; get initial positions
										(at ?block ?block-start))
			:effect (and
							(at ?dude ?block-start)  ; dude moves
							(not (at ?dude ?dude-start))
							(at ?block ?block-end)   ; block moves
							(not (at ?block ?block-start))
							(has-block ?block-end)
							(not (has-block ?block-start))
							))

	(:action move-block-down
			:parameters (?dude ?block ?dude-start ?block-start ?block-end)
			:precondition (and (dude ?dude)  ; get typing correct
										(block ?block) 
										(not (= ?dude-start ?block-start))
										(not (= ?block-start ?block-end))
										(not (= ?dude-start ?block-end))
										(space ?dude-start)
										(space ?block-start)
										(space ?block-end)
										(has-block ?block-start)
										(not (has-block ?dude-start))
										(not (has-block ?block-end))
										(is-above ?dude-start ?block-start)
										(is-above ?block-start ?block-end)
										(at ?dude ?dude-start) ; get initial positions
										(at ?block ?block-start))
			:effect (and
									(at ?dude ?block-start)  ; dude moves
									(not (at ?dude ?dude-start))
									(at ?block ?block-end)   ; block moves
									(not (at ?block ?block-start))
									(has-block ?block-end)
									(not (has-block ?block-start))
									))

	(:action move-block-left
			:parameters (?dude ?block ?dude-start ?block-start ?block-end)
			:precondition (and (dude ?dude)  ; get typing correct
										(block ?block) 
										(not (= ?dude-start ?block-start))
										(not (= ?block-start ?block-end))
										(not (= ?dude-start ?block-end))
										(space ?dude-start)
										(space ?block-start)
										(space ?block-end)
										(has-block ?block-start)
										(not (has-block ?dude-start))
										(not (has-block ?block-end))
										(is-left ?block-start ?dude-start)
										(is-left ?block-end ?block-start)
										(at ?dude ?dude-start) ; get initial positions
										(at ?block ?block-start))
			:effect (and
							(at ?dude ?block-start)  ; dude moves
							(not (at ?dude ?dude-start))
							(at ?block ?block-end)   ; block moves
							(not (at ?block ?block-start))
							(has-block ?block-end)
							(not (has-block ?block-start))
							))

	(:action move-block-right
			:parameters (?dude ?block ?dude-start ?block-start ?block-end)
			:precondition (and (dude ?dude)  ; get typing correct
										(block ?block) 
										(not (= ?dude-start ?block-start))
										(not (= ?block-start ?block-end))
										(not (= ?dude-start ?block-end))
										(space ?dude-start)
										(space ?block-start)
										(space ?block-end)
										(has-block ?block-start)
										(not (has-block ?dude-start))
										(not (has-block ?block-end))
										(is-left ?dude-start ?block-start)
										(is-left ?block-start ?block-end)
										(at ?dude ?dude-start) ; get initial positions
										(at ?block ?block-start))
			:effect (and
							(at ?dude ?block-start)  ; dude moves
							(not (at ?dude ?dude-start))
							(at ?block ?block-end)   ; block moves
							(not (at ?block ?block-start))
							(has-block ?block-end)
							(not (has-block ?block-start))
							)))