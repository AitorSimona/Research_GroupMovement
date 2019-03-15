# GroupMovement

“I am <https://www.linkedin.com/in/aitor-simona-bouzas-aab43817b/>(Aitor Simona), student of the
<https://www.citm.upc.edu/ing/estudis/graus-videojocs/>(Bachelor’s Degree in
Video Games by UPC at CITM). This content is generated for the second year’s
subject Project 2, under supervision of lecturer
<https://es.linkedin.com/in/ricardpillosu>(Ricard Pillosu).”

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

## Many Games Approach to Group Movement


