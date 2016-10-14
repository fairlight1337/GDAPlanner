(define (domain tablesetting)
  (:requirements :typing)
  (:types object location)
  (:predicates (at ?object - object ?location - location))
  (:constants)
  (:functions (location-of ?object) - location
              (total-cost) - number)
  (:action pick-object
	   :parameters (?object - object)
	   :precondition (and (found-object ?object)
			      (free-hand ?hand))
	   :effect (and (= (location-of ?object) ?location)
			(not (at ?object ?location))
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
	   :precondition (not (found-object ?object))
	   :effect (and (found ?object)
			(= (location-of ?object) ?location)
			(at ?object ?location))))
