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
Vezi `ostep-homework/threads-locks-usage` pentru diferite implementari de structuri de date ce ruleaza pe mai multe thread-uri.

## Condition variables
Putem vedea `pthread_cond_t`, conceptual, ca o coada de asteptare pentru thread-uri. Un pseudo-cod pentru cum sunt folosite de obicei **variabilele conditionale** este:
```c
	int done = 0;
T1:
	pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t c = PTHREAD_COND_INITIALIZER;
	pthread_mutex_lock(&m);
	/* Cod */
	while (done == 0)
		pthread_cond_wait(&c, &m); /* Il adauga pe T1 intr-o coada de asteptare (face automat unlock la m) */

T2 (in paralel):
	/* Cod */
	pthread_mutex_lock(&m);
	done = 1;
	pthread_cond_signal(&c);
	pthread_mutex_unlock(&m);
```

### Exemplu de folosire a variabilelor conditionale 
Sa zicem ca avem o pereche de thread-uri care scriu date intr-un buffer si o pereche de thread-uri care prelucreaza datele din buffer (eventual le scot dupa ce le prelucreaza). Pentrru acest scenariu o sa avem un lock principal (pentru buffer) si 2 variabile conditionale, numite c_plin si c_gol. O sa numim producatori thread-urile care baga date in buffer si consumatori pe cei care scot datele din buffer. Cand buffer-ul este plin, producatorii trebuie sa doarma, iar cand buffer-ul este gol consumatorii trebuie sa doarma. Pentru a rezolva asta avem:
 * consumatorul adoarme cu wait(&c_gol, &m) - doarme in coada c_gol pana cand se da semnal de la cineva ca s-au pus date in buffer
 * producatorul adoarme cu (&c_plin, &m) - doarme in coada c_plin pana cand se da semnal de la cineva ca s-a eliberat spatiu din buffer

Daca un producator baga ceva in buffer, el va face signal(&c_gol), iar daca un consumator ia date din buffer, acesta va face signal(&c_plin).

### `Producer/Consumer - Bounded Buffer Problem`
> [NOTE!] Pentru un exemplu detaliat + cod vezi `Chapter 30 page 6`.

Principala problema este ca **signal** trece starea unui thread din sleeping in ready, **NU** in running. De aici pot aparea diferite probleme de sincronizare intre un producator si mai multi consumatori.

#### `The Single Buffer Producer/Consumer Solution:`
```c
int buffer;
int count = 0; // initially, empty

void put(int value) {
	assert(count == 0);
	count = 1;
	buffer = value;
}

int get() {
	assert(count == 1);
	count = 0;
	return buffer;
}

int loops; // must initialize somewhere...
cond_t empty, fill;
mutex_t mutex;

void *producer(void *arg) {
	int i;
	for (i = 0; i < loops; i++) {
		Pthread_mutex_lock(&mutex);
		while (count == 1)
			Pthread_cond_wait(&empty, &mutex);
		put(i);
		Pthread_cond_signal(&fill);
		Pthread_mutex_unlock(&mutex);
	}
}
void *consumer(void *arg) {
	int i;
	for (i = 0; i < loops; i++) {
		Pthread_mutex_lock(&mutex);
		while (count == 0)
			Pthread_cond_wait(&fill, &mutex);
		int tmp = get();
		Pthread_cond_signal(&empty);
		Pthread_mutex_unlock(&mutex);
		printf("%d\n", tmp);
	}
}
```
> [NOTE!] Aceasta solutie rezolva 2 probleme: (1) faptul ca atunci cand se apeleaza signal(), procesul din coada este trecut din sleeping in ready (din (if count == val) in while (count == val)); (2) folosirea a 2 variabile conditionale, una pentru consumator si una pentru producator. Vezi `Chapter 30 page 6-13` pentru explicatii suplimentare la ce ar putea merge prost la implementarile de dinaintea acesteia.

#### `The Single Buffer Producer/Consumer Solution (la modul general):`
```c
int buffer[MAX];
int fill_ptr = 0;
int use_ptr = 0;
int count = 0;

void put(int value) {
	buffer[fill_ptr] = value;
	fill_ptr = (fill_ptr + 1) % MAX;
	count++;
}
int get() {
	int tmp = buffer[use_ptr];
	use_ptr = (use_ptr + 1) % MAX;
	count--;
	return tmp;
}

int loops; // must initialize somewhere...
cond_t empty, fill;
mutex_t mutex;

void *producer(void *arg) {
	int i;
	for (i = 0; i < loops; i++) {
		Pthread_mutex_lock(&mutex);
		while (count == MAX)
			Pthread_cond_wait(&empty, &mutex);
		put(i);
		Pthread_cond_signal(&fill);
		Pthread_mutex_unlock(&mutex);
		}
}

void *consumer(void *arg) {
	int i;
	for (i = 0; i < loops; i++) {
		Pthread_mutex_lock(&mutex);
		while (count == 0)
			Pthread_cond_wait(&fill, &mutex);
		int tmp = get();
		Pthread_cond_signal(&empty);
		Pthread_mutex_unlock(&mutex);
		printf("%d\n", tmp);
	}
}
```

> [NOTE!] Exista si un apel de broadcast() ce trezeste mai multe thread-uri deodata, astfel rezolvand problema (2) descrisa in carte, insa overhead-ul poate fi unul mare.

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
 * When checking for a condition in a multi-threaded program, using a
while loop is always correct; using an if statement only might be, depending on the semantics of signaling. Thus, always use while and your
code will behave as expected.
Using while loops around conditional checks also handles the case where
spurious wakeups occur. In some thread packages, due to details of the
implementation, it is possible that two threads get woken up though just
a single signal has taken place [L11]. Spurious wakeups are further reason
to re-check the condition a thread is waiting on.
 * We’ve now seen two examples of initializing a semaphore. In the first
case, we set the value to 1 to use the semaphore as a lock; in the second,
to 0, to use the semaphore for ordering. So what’s the general rule for
semaphore initialization?
One simple way to think about it, thanks to Perry Kivolowitz, is to consider the number of resources you are willing to give away immediately
after initialization. With the lock, it was 1, because you are willing to
have the lock locked (given away) immediately after initialization. With
the ordering case, it was 0, because there is nothing to give away at the
start; only when the child thread is done is the resource created, at which
point, the value is incremented to 1. Try this line of thinking on future
semaphore problems, and see if it helps.
