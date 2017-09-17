# mad-starter
A simple [Coders Strike Back](https://www.codingame.com/multiplayer/bot-programming/coders-strike-back) search-based starter bot that has a fully working physics simulator but almost no logic.
It can easily get to legend league if you put some simple logic inside.

# Usage

Code assumes input format provided in Gold or Legend league and will not work for lower leagues.  
If you are in Gold or Legend league, all you need to do is copy/paste and submit. 

# Next steps

If you want to get further in the ranking, you'll need to start adding improvements to it. Here are some ideas in order of difficulty to implement:

* Add more parameters (or *change existing*) in evaluation function
* Add different coefficients to evaluation parameters
* Add bots with alternative coefficients as opponents for your main bot
* Replace solver with Genetic Algorithm based (**most impact**)
* Replace computationally heavy math functions with cache or approximations
* Rewrite reflex bot implementation

# Additional resources

* [Magus post-mortem](http://files.magusgeek.com/csb/csb_en.html) (bulk of API and collision code is based on this)
* [pb4 post-mortem](https://www.codingame.com/blog/coders-strike-back-pb4608s-ai-rank-3rd/)
* [Jeff06 post-mortem](https://www.codingame.com/blog/genetic-algorithms-coders-strike-back-game/)
* [sethorizer's sim engine tester](https://github.com/sethorizer/csb)