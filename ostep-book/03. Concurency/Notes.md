# Concurency

## Locks
Daca T1 face lock pe un mutex iar apoi apare T2 care vrea si el sa faca lock (T1 nu a dat inca unlock) T2 este mutat din starea **Running** in **Sleeping** pana cand mutex-ul este eliberat.

Exemple de algoritmi de implementare a unui lock (spinlock() - `busy waiting`):
```c
/* Peterson's algortihm */
/* In practica nu va merge din cauza optimizarilor
 din spate pe care le poate face compiler-ul.*/
int flag[2];
int turn;
void init() {
	// indicate you intend to hold the lock w/ ’flag’
	flag[0] = flag[1] = 0;
	// whose turn is it? (thread 0 or 1)
	turn = 0;
}

void lock() {
	// ’self’ is the thread ID of caller
	flag[self] = 1;
	// make it other thread’s turn
	turn = 1 - self;
	while ((flag[1-self] == 1) && (turn == 1 - self))
	; // spin-wait while it’s not your turn
}

void unlock() {
	// simply undo your intent
	flag[self] = 0;
}
```

```c
/* Suport hardware pentru operatie atomica. Asa arata pseudocodul C
 (tianand) cont ca operatiile se executa atomic. Pentru o 
 implementare reala in C a CcompareAndSwap() vezi codul
 din ostep-code/threads-lock */
int TestAndSet(int *old_ptr, int new) {
	int old = *old_ptr; // fetch old value at old_ptr
	*old_ptr = new; // store ’new’ into old_ptr
	return old; // return the old value
}
int CompareAndSwap(int *ptr, int expected, int new) {
	int original = *ptr;
	if (original == expected)
		*ptr = new;
	return original;
}

typedef struct __lock_t {
	int flag;
} lock_t;

void init(lock_t *lock) {
	// 0: lock is available, 1: lock is held
	lock->flag = 0;
}

void lock(lock_t *lock) { // Folosind TestAndSet()
	while (TestAndSet(&lock->flag, 1) == 1)
	; // spin-wait (do nothing)
}

void lock(lock_t *lock) { // Folosind CompareAndSwap()
	while (CompareAndSwap(&lock->flag, 0, 1) == 1)
	; // spin
}

void unlock(lock_t *lock) {
	lock->flag = 0;
}
```

Exemplu de implementare a unui algoritm de locking, nefiind bazat pe busy waiting (face putin busy waiting, insa nu la fel de mare ca si primele solutii):
```c
/* Primitive oferite de OS
	park() - pune un thread in sleep
	setpark() - indica faptul ca un thread este pe cale sa apeleze park()
	unpark(threadID) - trezeste thread-ul cu ID-ul respectiv
*/
typedef struct __lock_t {
	int flag;
	int guard;
	queue_t *q;
} lock_t;

void lock_init(lock_t *m) {
	m->flag = 0;
	m->guard = 0;
	queue_init(m->q);
}

void lock(lock_t *m) {
	while (TestAndSet(&m->guard, 1) == 1)
	; //acquire guard lock by spinning

	if (m->flag == 0) {
		m->flag = 1; // lock is acquired
		m->guard = 0;
	} else {
		queue_add(m->q, gettid());
		setpark();
		m->guard = 0;
		park();
	}
}

void unlock(lock_t *m) {
	while (TestAndSet(&m->guard, 1) == 1)
	; //acquire guard lock by spinning

	if (queue_empty(m->q))
		m->flag = 0; // let go of lock; no one wants it
	else
		unpark(queue_remove(m->q)); // hold lock
		// (for next thread!)

	m->guard = 0;
}
```

Exemplu din lumea reala de implementare:
```c
void mutex_lock (int *mutex) {
	int v;
	// Bit 31 was clear, we got the mutex (fastpath)
	if (atomic_bit_test_set (mutex, 31) == 0)
		return;

	atomic_increment (mutex);
	while (1) {
		if (atomic_bit_test_set (mutex, 31) == 0) {
			atomic_decrement (mutex);
		return;
	}

	// Have to waitFirst to make sure futex value
	// we are monitoring is negative (locked).
	v = *mutex;
	if (v >= 0)
		continue;
		futex_wait (mutex, v);
	}
}

void mutex_unlock (int *mutex) {
	// Adding 0x80000000 to counter results in 0 if and
	// only if there are not other interested threads
	if (atomic_add_zero (mutex, 0x80000000))
		return;

	// There are other threads waiting for this mutex,
	// wake one of them up.
	futex_wake (mutex);
}
```

## Locked data structures
`Chapter 29` - liste, cozi si hash-table

## Notes from the book
 * A critical section is a piece of code that accesses a shared resource,
usually a variable or data structure.
 * A race condition (or data race [NM92]) arises if multiple threads of
execution enter the critical section at roughly the same time; both
attempt to update the shared data structure, leading to a surprising
(and perhaps undesirable) outcome.
 * An indeterminate program consists of one or more race conditions;
the output of the program varies from run to run, depending on
which threads ran when. The outcome is thus not deterministic,
something we usually expect from computer systems.
 * To avoid these problems, threads should use some kind of mutual
exclusion primitives; doing so guarantees that only a single thread
ever enters a critical section, thus avoiding races, and resulting in
deterministic program outputs.