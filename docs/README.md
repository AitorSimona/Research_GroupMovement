# GroupMovement

I am <https://www.linkedin.com/in/aitor-simona-bouzas-aab43817b/>(Aitor Simona), student of the
<https://www.citm.upc.edu/ing/estudis/graus-videojocs/>(Bachelor’s Degree in Video Games by UPC at CITM). 
This content is generated for the second year’s subject Project 2, under supervision of lecturer
<https://es.linkedin.com/in/ricardpillosu>(Ricard Pillosu).

* My Github Account: [AitorSimona](https://github.com/AitorSimona)

Welcome! I understand you are interested in this topic, one of the cores of the RTS genre!
So what is Group Movement? 

Group Movement: Moving multiple units at the same time through complex terrain, while taking into account the relationships between the units that are part of the group.

Wow, such a generic definition, don't you think? Let us expand it a bit. We are speaking about two big things here...

* Groups
* Movement

## Groups

When we have to deal with more than one unit things get quite complicated, it is no longer a matter of Unit A following the orders we give  it. We are dealing with a group of units, Unit A has to follow the orders received while dealing with those orders its colleagues are receiving, at the same time! 

So, what if we give the order to move from position A to position B to all the units in our group. Will each unit follow its own path towards the destination? But if they do so, they won't be behaving as if they were a group, so we need to keep them together while advancing, right?

Here is where many problems arise. How do we handle this group, how do we keep them together, move them apart each other when colliding, while maintaining a structured formation? My my, what a bunch of questions. There are so many things going on in the interactions between the units and the world and between the units itselves, that we could make a long list of questions of this type. 

So let us keep with the keypoint here: **How do we keep units organised, in a structured way?**

## Movement

Now, if we give the order to move from position A to position B to all the units in our group. We need a way to get a path from A to B, and here we encounter another topic, Pathfinding. Pathfinding is not the main topic of this research, but is is necessary for the sake of it, so we will be using a tile-based greedy algorithm well-known called A*. We will use a very basic implementation of it, but more than enough to enable our units movement. 

So, A* can give us a path, problem solved right? Not at all close. While A* gives us a path from A to B, it takes a good amount of time for it to retrieve this path. This problem is not so big when dealing with a single unit, but remember, we are dealing with groups...

As you may be infering already, we have a big problem, our A* will take too much time to compute a path for every single unit of a large group, so we would need some optimizations to reduce the CPU load our current approach has. We will not go into this optimizations, since it is not the topic we are discussing here, I just want you to be aware of the problem.

Our new keypoint: **How do we move units from one position to another?**

## Movement in Groups

I hope you have a clear idea of our main objectives now, If I were to sum up what I explained in the last two sections ...

Our main keypoint: **How do we move units from one position to another, while keeping them organised, in a structured way?**

***

## Many Games Approach to Group Movement

As always, it is not easy to find this kind of information, technical aspects of games, revenue... I was able to find a decent piece of info looking into the GDC vault, specifically into 2011 conferences. Does it ring a bell, 2011? A famous RTS launched very close to that date...


### Starcraft II

Bingo! Blizzard's Starcraft 2 launched the 27th of July 2010, and in the next GDC James Anhalt (Lead Software Engineer at Blizzard) shed some light on Starcraft 2's tech. He goes into group movement stuff, such as Steering behaviours which I will mention later. 
The other games present in this conference were Heroes on the Move from nStigate Games and Dragon Age Origins from Bioware. They all went deep into pathfinding too, apart from our main topic. It is not strange, since dealing with pathfinding is one of the biggest headaches of the RTS genre.

Click to see the full GDC 2011 talk:  [GDC 2011](https://www.gdcvault.com/play/1014514/AI-Navigation-It-s-Not)

<iframe width="560" height="315" src="https://www.youtube.com/embed/F6h0-uW4JZI" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>


### Supreme Commander II

Quite a big name too. In this case Supreme Commander departs from antoher way to approach crowd behaviours, pathfinding. They use a modern way of pathfinding called "Flow Field", also known as Vector Field or Force Field to others. I won't go into detail since it is related to pathfinding but in SC2 they base their group movement on this technique.

Take a look at the following video to see a more accurate view of SC2 Flowfield and crowd movement, narrated by Chris Taylor from Gas Powered Games.

<iframe width="560" height="315" src="https://www.youtube.com/embed/iHuFCnYnP9A" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

### Others

I have read that Planetary Annhilation also uses SC2 Flowfield tech, while other old games like Age of Empires seem to use a modification of A*, sadly couldn't find anything on games like the Total War series or the Wargame Series which I like a lot, but as I said information like this is rarely shared in the industry.

***

## How do we approach Group Movement?

Now that the topic has already been slightly introduced, let's look more deeply into the matter beign discussed.

In Crowd-based simulations we can differentiate 3 approaches:

* Flow-based: The crowd as a whole rather than its components. Individuals are equal and behavioural factors are heavily reduced
* Entity-based: All movements are determined by some global laws enforced to the individuals of the group
* Agent-based: Autonomous intelligent individuals. Action-reaction is local to each individual based on a set of rules.

Behavioural factors, rules, laws... Very similar wording, right? Indeed, they all refer to the same concept, called Steering behaviours.

In 1986 Craig Reynolds, a software engineer, expert in artificial life and computer graphics, made a computer model that simulated coordinated animal motion, such as the movement of bird flocks and fish schools. In his words:

"I called the generic simulated flocking creatures boids. The basic flocking model consists of three simple steering behaviors which describe how an individual boid maneuvers based on the positions and velocities of its nearby flockmates".

Boids three Steering behaviours as defined by Craig Reynolds himself, which together are defined as **flocking**:

Separation: steer to avoid crowding local flockmates.

<p align="center"><img src="https://github.com/AitorSimona/Research_GroupMovement/blob/master/docs/Images/separation.png"></p>

Alignment: steer towards the average heading of local flockmates.

<p align="center"><img src="https://github.com/AitorSimona/Research_GroupMovement/blob/master/docs/Images/alignment.png"></p>

Cohesion: steer to move toward the average position of local flockmates.

<p align="center"><img src="https://github.com/AitorSimona/Research_GroupMovement/blob/master/docs/Images/cohesion.png"></p>

The boids also have a region on which they have influence over neighboring boids, this region is defined by an angle and distance, building a spherical field around each boid.

<p align="center"><img src="https://github.com/AitorSimona/Research_GroupMovement/blob/master/docs/Images/neighborhood.png"></p>

_All images are property of Craig Reynolds, his web's link is at the bottom of this page_

So we could define this steering behaviours as **a set of rules that regulate the relationship between individuals of a group**.
By Craig Reynolds himself: _"the ability to navigate around their world in a life-like and improvisational manner."_

### Steering Behaviours

So, how many rules can we set to keep our units moving in a structured way? As defined by Craig Reynolds...

* Simple behaviors for individuals and pairs:

  * Seek and Flee
  * Pursue and Evade
  * Wander
  * Arrival
  * Obstacle Avoidance
  * Containment
  * Wall Following
  * Path Following
  * Flow Field Following
  
* Combined behaviors and groups:

  * Crowd Path Following
  * Leader Following
  * Unaligned Collision Avoidance
  * Queuing (at a doorway)
  * Flocking (combining: separation, alignment, cohesion)
  
Wow, right? He defined lots of rules to enforce behaviours into groups, he himself as a precursor to everything videogames have been implementing ever since. 

I'm not going to go one by one explaining each single behaviour since I haven't programmed them and we would extend this way too much, but if you are interested you can use this link: [Steering Behaviours](https://gamedevelopment.tutsplus.com/tutorials/understanding-steering-behaviors-leader-following--gamedev-10810) to gain a better understanding of some of them. Fernando Bevilacqua has some very good visuals to support the explanation.
  
