#!/bin/bash
RUN_DIFF=$2

if [ -d "td/" ]; then
	echo "Writing..."
else
	echo "td not exists, create td"
	mkdir td/
fi

if ! [ -f "dofiles/$1" ]; then
	echo "Test files do not exists!"
	exit 0
fi

echo "===== Running mine ===="
./fraig -f dofiles/$1 >& td/$1_t.log

if [ -f "simRes.log" ]; then
	echo "Rename mine simulation file..."
	mv simRes.log td/$1_t_simRes.log
fi

echo "done."

echo "===== Running ref ====="
./ref/fraig -f dofiles/$1 >& td/$1_r.log

if [ -f "simRes.log" ]; then
	echo "Rename ref simulation file..."
	mv simRes.log td/$1_r_simRes.log
fi

echo "done."

if [ $RUN_DIFF ]; then
	echo "===== Running diff ====="
	colordiff td/$1_t.log td/$1_r.log
	echo "diff finished"
fi
