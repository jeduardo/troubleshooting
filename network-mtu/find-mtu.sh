#!/usr/bin/env bash

# Google DNS target
TARGET=8.8.8.8
# Normal default MTU size
START_MTU=1500
# Timeout (seconds)
TIMEOUT=1
# Packet header - 20 bytes TCP header + 8 bytes ICMP header
SIZE=28
# Preferred protocol
PROTO='4'

if [ ! -z "$1" ] && [ "$1" == '-6' ]; then
  PROTO='6'
fi
if [ ! -z "$1" ] && [ "$1" != '-4' ] && [ "$1" != '-6' ]; then
  TARGET=$1
fi

if [ ! -z "$2" ]; then
  TARGET=$2
fi

FOUND=1
OPTIMAL_MTU=$START_MTU

trap cancel INT

function cancel() {
  echo "Cancelling"
  exit 1
}

echo "Target is ${TARGET}/IPv${PROTO}"

while (($FOUND > 0)); do
  PAYLOAD_SIZE=$(($OPTIMAL_MTU - $SIZE))
  if (($PAYLOAD_SIZE <= 0)); then
    echo "Limit exceeded, something is wrong"
    exit 1
  fi
  echo "Testing MTU: $OPTIMAL_MTU"
  ping -${PROTO} -s $PAYLOAD_SIZE -M do $TARGET -W $TIMEOUT -c 1 | grep '1 received'
  FOUND=$?
  if (($FOUND > 0)); then
    let "OPTIMAL_MTU-=1"
  fi
  exit 0
done

echo "Optimal MTU size is $OPTIMAL_MTU"
