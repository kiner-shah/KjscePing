## TODO
~~ While loop for echo requests ~~
~~ Implement infinite or count-based pinger ~~
~~ Time between request and response to be logged ~~
~~ Proper socket close handling ~~
~~ Call ::shutdown() when there is a timeout to close the connection ~~

### Experiment?
- Make it such that a new socket is created for each request
- Consider using std::async instead of a continuously running thread
