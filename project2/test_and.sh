#!/usr/bin/env bash

# executable variables
#
SERVER="text-server"
#
CLIENT="text-client"

# execution parameters
#
SOCKET_NAME="csv_server_and_test"
FILE="dat/bankloan2.csv"
SEARCH_STR="indus x farm x default"

# output variables
#
SERVER_LOG="test_and_server.txt"
SERVER_FILES="$SERVER $SERVER_LOG"
#
CLIENT_LOG="test_and_client.txt"
CLIENT_FILES="$CLIENT $CLIENT_LOG"

# start server
#
echo "./$SERVER $SOCKET_NAME &> $SERVER_LOG &"
./$SERVER $SOCKET_NAME &> $SERVER_LOG &
# remember PID to kill later
SERVER_PID=$!

sleep 1

# start client
#
echo "./$CLIENT $SOCKET_NAME $FILE $SEARCH_STR &> $CLIENT_LOG"
./$CLIENT $SOCKET_NAME $FILE $SEARCH_STR &> $CLIENT_LOG

sleep 3

# kill server
#
kill $SERVER_PID
