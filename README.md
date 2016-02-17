# UE4-Integrated-Demo

LEET Multiplayer Integrated Demo.  

Full How-to coming soon!

Objective:

Build a game in Unreal Engine 4 which uses (most of) the LEET API.

Detail:

LEET provides a complete API to tie in directly with your game.  Activating and deactivating players, submission of match results, and chat replication are covered.

Strategy:

We will be building a simple 2 player game.  The players will meet on the leet website.  When they sign up for a match, a new dedicated server will be created and configured.  The players will then connect to the server instance.  Players have a projectile which they fire at each other.  4 hits with the projectile awards a point.  3 points is a win and the game is closed.  The dedicated server then notifies the leet website, and the player's ranks and other settings are modified.

Requirements:

A developer account on the LEET sandbox:  
Unreal Engine 4.11 or higher built form the github source.
Windows 7 or higher with MS Visual Studio 2015