#!/user/bin/env bash

# executable variables
#
SERVER="text-server"
#
CLIENT="text-client"

# clean things up before beginning
#
rm $SERVER_FILES
rm $CLIENT_FILES
make clean

# build applications
make $SERVER
make $CLIENT


# test 1 execution parameters
#
SOCKET_NAME1="csv_server1"
FILE1="dat/bankloan1.csv"
SEARCH_STR1="default"

# output variables
#
SERVER_MSG1="exe1_server_out.txt"
SERVER_LOG1="exe1_server_log.txt"
SERVER_FILES1="$SERVER $SERVER_MSG1 $SERVER_LOG1"
#
CLIENT_MSG1="exe1_client_out.txt"
CLIENT_LOG1="exe1_client_log.txt"
CLIENT_FILES="$CLIENT $CLIENT_MSG1 $CLIENT_LOG1"

# start server
#
echo "./$SERVER $SOCKET_NAME1 1> $SERVER_MSG1 2> $SERVER_LOG1 &"
./$SERVER $SOCKET_NAME1 1> $SERVER_MSG1 2> $SERVER_LOG1 &
# remember PID to kill later
SERVER_PID=$!

# start client
#
echo "./$CLIENT $SOCKET_NAME1 $FILE1 $SEARCH_STR1 1> $CLIENT_MSG1 2>
  $CLIENT_LOG1"
./$CLIENT $SOCKET_NAME1 $FILE1 $SEARCH_STR1 1> $CLIENT_MSG1 2> $CLIENT_LOG1

sleep 1

# kill server
#
kill $SERVER_PID


# server exe variables
#
SERVER_MSG2="exe2_server_out.txt"
SERVER_LOG2="exe2_server_log.txt"
SERVER_FILES1="$SERVER $SERVER_MSG2 $SERVER_LOG2"

# client exe variables
#
CLIENT_MSG2="exe2_client_out.txt"
CLIENT_LOG2="exe2_client_log.txt"
# test 2 execution parameters
#
SOCKET_NAME2="csv_server2"
FILE2="dat/bankloan1.csv"
SEARCH_STR2="default + industrial"

# test 2 master files
#
# start server
#
echo "./$SERVER $SOCKET_NAME2 1> $SERVER_MSG2 2> $SERVER_LOG2 &"
./$SERVER $SOCKET_NAME2 1> $SERVER_MSG2 2> $SERVER_LOG2 &
# remember PID to kill later
SERVER_PID2=$!

# start client
#
echo "./$CLIENT $SOCKET_NAME2 $FILE2 $SEARCH_STR2 1> $CLIENT_MSG2 2> $CLIENT_LOG2"
./$CLIENT $SOCKET_NAME2 $FILE2 $SEARCH_STR2 1> $CLIENT_MSG2 2> $CLIENT_LOG2

# kill server
#
kill $SERVER_PID2
