# Come teststare

L'opzione **-D** serve a far partire il programma in foreground e vedere l'output


Per far partire il server lato gestore
```
sudo slurmctld -v -D
```


Per far partire il server lato client
```
sudo slurmctld -v -D
```


Konro start

```
./konro
```


env variable

```
export OMP_TOOL_LIBRARIES=/usr/local/lib/libdlb.so
export DLB_ARGS="--drom --ompt --ompt-thread-manager=none --talp=openmp --talp-external-profiler"
```

Per farlo andare
```
./runner ./backprop 16500000
srun -n 1 --overlap --ntasks-per-core=1 -v  ./runner ./backprop 1650000
```



DLB_CHECK 

```
watch dlb_shm -l
```



