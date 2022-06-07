# Yakikani, a Wanikani's burned kanji trainer

## Presentation

This project is a server-client C language system to perform review of burned kanji and vocabulary from Wanikani.

The server part is located inside _./server/_ directory.
The client part is located inside _./client/_ directory.

There is also a Javascript project inside _./server/dataScript/_ to create or update the items database located inside the JSON file in _./server/data/_ directory.

A part for the parsing of JSON file by the server is made by using [DaveGamble cJSON](https://github.com/DaveGamble/cJSON#including-cjson) library (located in _./server/src/cJSON.c_ and _./server/include/cJSON.h_), an open source C library for JSON parsing.

## How to use

### Compilation

A Makefile is located at the root of the project. You can create both server and client executables or delete object files using this file with the following commands :

To run compilation

```bash
> make
```

To delete all .o files

```bash
> make clean
```

To delete all .o files and executables

```bash
> make fclean
```

### Run programs

Both server and client are run by passing the IP address and the port number as parameter :

```bash
> cd ./server/ && ./kanjiTrainerServ 127.0.0.1 8888
```

```bash
> cd ./client/ && ./kanjiTrainerClient 127.0.0.1 8888
```

Client can ask a review session with the following command :

```bash
> ===> !start 10
```

The parameter indicate the number of item that the client wants to review, it must be between 1 and 50.

The client can quit the program at any moment (even inside a review) with this command :

```bash
> ===> !exit
```

### dataScript

The Javascript used to populate the database can also be run but a Wanikani V2 token is required.

First you need to put a valid Wanikani V2 token inside _./server/dataScript/.env_ file :

```txt
LAST_UPDATE= //no need to fill this line
API_TOKEN= //put your Wanikani V2 token here
```

To run the program use the following commands :

```bash
> cd ./server/dataScript/
> npm install
> npm run start
```
