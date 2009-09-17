(define (domain sokoban-domain)
	(:requirements :equality)
	(:predicates (space ?x)       ; x is a grid square coordinate, and is movable space
							 (is-left ?x ?y)  ; x, y are grid squares, and x is to the left of y
							 (is-above ?x ?y) ; x, y are grid squares, and x is above y
							 (at ?x ?y)       ; object x is on grid square y
							 (clear ?x)				; grid space x is clear
							 (dude ?x)				; object x is the small robot thing
							 (block ?x))			; object x is a block
	(:action move; move without pushing anything
			:parameters (?dude ?start ?end)
			:precondition (and (dude ?dude)            
												 (or ; could be any movement direction
														(is-above ?start ?end)
														(is-above ?end ?start)
														(is-left ?start ?end)
														(is-left ?end ?start))
												 (at ?dude ?start)
												 (space ?start)
												 (space ?end)
												 (clear ?end)
												 (not (clear ?start)))
			:effect (and 
								(at ?dude ?end)
								(clear ?start)
								(not (clear ?end))
								(not (at ?dude ?start))))
	(:action move-block
			:parameters (?dude ?block ?dude-start ?block-start ?block-end)
								:precondition (and (dude ?dude)  ; get typing correct
																	 (block ?block) 
																	 (space ?dude-start)
																	 (space ?block-start)
																	 (space ?block-end)
																	 (clear ?block-end)
																	 (or  ; check linear path and adjacent spaces - note: spaces cannot be both left and above
																			(and (is-above ?block-start ?dude-start)
																					 (is-above ?block-end ?block-start))
																			(and (is-above ?dude-start ?block-start)
																					 (is-above ?block-start ?block-end))
																			(and (is-left ?block-start ?dude-start)
																					 (is-left ?block-end ?block-start))
																			(and (is-left ?dude-start ?block-start)
																					 (is-left ?block-start ?block-end)))
																	 (at ?dude ?dude-start) ; get initial positions
																	 (at ?block ?block-start)
																	 (not (clear ?block-start))
																	 (clear ?block-end))
								:effect (and
														(at ?dude ?block-start)  ; dude moves
														(not (at ?dude ?dude-start))
														(at ?block ?block-end)   ; block moves
														(not (at ?block ?block-start))
														(clear ?dude-start) ; spaces free
														(not (clear ?block-start))
														(not (clear ?block-end))
														)))