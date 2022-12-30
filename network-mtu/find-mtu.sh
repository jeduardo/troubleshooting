#!/usr/bin/env bash

# Google DNS target
TARGET=8.8.8.8
# Normal default MTU size
START_MTU=1500
# Timeout (seconds)
TIMEOUT=1
# Packet header
SIZE=28

FOUND=1
OPTIMAL_MTU=$START_MTU

while (($FOUND > 0)); do
  PAYLOAD_SIZE=$(($OPTIMAL_MTU - $SIZE))
  if (($PAYLOAD_SIZE <= 0)); then
    echo "Limit exceeded, something is wrong"
    exit 1
  fi
  echo "Testing MTU: $OPTIMAL_MTU"
  ping -s $PAYLOAD_SIZE -M do $TARGET -W $TIMEOUT -c 1 | grep '1 received'
  FOUND=$?
  if (($FOUND > 0)); then
    let "OPTIMAL_MTU-=1"
  fi
done

echo "Optimal MTU size is $OPTIMAL_MTU"
