#!/bin/bash

NTIME = `TIME=$(date +"%Y-%m-%d %T)"; echo $TIME | tr -s " " " "`
PUSH_FILE = "./log.txt"

echo $QUERY_STRING $REMOTE_ADDR $NTIME >> $PUSH_FILE

REPONSE="{\"Shipment\":\"OK\"}"


echo "Content-type: application/json"
echo ""
echo "{\"message\": \"received\"}"

exit 0