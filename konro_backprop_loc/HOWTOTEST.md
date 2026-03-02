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
export  DLB_ARGS="--ompt --verbose=ompt --ompt-thread-manager=omp5 --drom"
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



