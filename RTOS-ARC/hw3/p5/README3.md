# run command to start main ARC if not already started
we will  run gui from main node, (this one) and connect to it using second node
`/home/cbsavugo/RTOS-ARC/tools/set-arc-node.ps1`



# start vnc vewing
```bash
/home/cbsavugo/RTOS-ARC/tools/start_arc_gui.sh
```

## In CMD Terminal Window #2
Connect to Arc
Command:
```bash 
ssh -L 5901:<terminal_1-node>:5901 <arc ssh key> # Example 
ssh -L 5901:c78:5901 arc # Example 
```

Output:
```bash
===================================================
               Welcome to ARC V3
===================================================
Notice: Do NOT use srun, use salloc instead.
See https://arcb.csc.ncsu.edu/~mueller/cluster/arc/
===================================================
Last login: Thu Feb  5 18:45:51 2026 from 10.137.228.158
[cbsavugo@login2 ~]$ ssh -L 5901:c78:5901 arcv
```