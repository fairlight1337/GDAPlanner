(define (domain simple-tablesetting)
  (:requirements :typing)
  (:types object location hand)
  (:action pick-object
	   :parameters (?object - object)
	   :precondition (and (found ?object)
			      (free-hand ?hand)
                              (at ?object ?location))
	   :effect (and (not (at ?object ?location))
			(in-hand ?object ?hand)
			(not (free-hand ?hand))))
  (:action place-object
	   :parameters (?object - object
			?location - location)
	   :precondition (in-hand ?object ?hand)
	   :effect (and (not (in-hand ?object ?hand))
			(free-hand ?hand)
			(at ?object ?location)))
  (:action find-object
	   :parameters (?object - object)
	   :precondition (not (found ?object))
	   :effect (and (found ?object))))
