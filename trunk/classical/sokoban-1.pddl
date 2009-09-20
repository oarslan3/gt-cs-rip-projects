(define (problem sokoban-1)
	(:domain sokoban-domain)
	(:objects player ; the dude
						A      ; the block
						p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 p11 p12 p13) ; the locations
	(:init 
			; initialize object types
			(dude player)
			(block A)
			(space p1) (space p2) (space p3) (space p4) (space p5) (space p6)
			(space p7) (space p8) (space p9) (space p10) (space p11) (space p12) (space p13)
			; initialize relationships between spaces
			; --- row relationships
			(is-left p1 p2) 
			(is-left p4 p5) (is-left p5 p6) (is-left p6 p7)
			(is-left p8 p9) (is-left p9 p10) (is-left p10 p11)
			(is-left p12 p13)
			; --- column relationships
			(is-above p4 p8) (is-above p8 p12)
			(is-above p9 p13) (is-above p5 p9) (is-above p3 p5) (is-above p2 p3)
			(is-above p6 p10) 
			(is-above p7 p11)
			; place the objects
			(at player p3)
			(has-dude p3)
			(at A p10)
			(has-block p10)
			; clear everything else 
			;(clear p1) (clear p2) (clear p4) (clear p5) (clear p6) (clear p7)
			;(clear p8) (clear p9) (clear p11) (clear p12) (clear p13)
			)
	(:goal (and
						(at A p2)
						(has-block p2))))