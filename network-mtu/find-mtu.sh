#!/usr/bin/env bash

# Google DNS target
TARGET=8.8.8.8
# Normal default MTU size
START_MTU=1500
# Timeout (seconds)
TIMEOUT=1
# ICMP header: 8 bytes for both IPv4 and IPv6.
ICMP_HEADER=8
# Preferred protocol
PROTO='4'
IP_HEADER=20

if [ ! -z "$1" ] && [ "$1" == '-6' ]; then
  PROTO='6'
  IP_HEADER=40
fi
if [ ! -z "$1" ] && [ "$1" != '-4' ] && [ "$1" != '-6' ]; then
  TARGET=$1
fi

if [ ! -z "$2" ]; then
  TARGET=$2
fi

FOUND=1
OPTIMAL_MTU=$START_MTU
IF=$(ip -${PROTO} route | grep default | cut -d ' ' -f 5)


trap cancel INT

function cancel() {
  echo "Cancelling"
  exit 1
}

echo "Target is ${TARGET}/IPv${PROTO} over interface ${IF}"

while (($FOUND > 0)); do
  SIZE=$(($ICMP_HEADER + $IP_HEADER))
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
done

echo "Optimal MTU size is $OPTIMAL_MTU"
