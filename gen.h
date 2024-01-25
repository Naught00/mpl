#ifndef GEN_H
#define GEN_H

/* Stack */
#define push(s, v) s.stack[s.sp++] = v
#define pop(s, v)  s.stack[s.sp--]
#define popl(s, v) s.sp--
#define top(s) s.stack[s.sp - 1]
#define stack_index(s, i) s.stack[i]
#define modtop(s, v) s.stack[s.sp - 1] = v

/* Mem pools */
#define pool(p)           &p.pool[p.p_index++]
#define pool_stay(p)      &p.pool[p.p_index]
#define pool_index(p, i)  &p.pool[i]
#define pool_offset(p, o) &p.pool[p.p_index + o]
#define pool_top(p)       &p.pool[p.p_index - 1]
#define pool_inc(p)       p.p_index++
#define pool_freetop(p)   p.p_index--

#endif
