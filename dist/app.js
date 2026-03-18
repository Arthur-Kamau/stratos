// Stratos Web — Generated App (Signal-based Reactivity)
'use strict';

// === Stratos Signal Runtime ===
// SolidJS-inspired fine-grained reactivity for the browser

let _currentEffect = null;
const _effectStack = [];
let _batchDepth = 0;
let _pendingEffects = [];

function createSignal(initial) {
  let value = initial;
  const subscribers = new Set();

  function get() {
    if (_currentEffect) subscribers.add(_currentEffect);
    return value;
  }

  function set(newValue) {
    if (value === newValue) return;
    value = newValue;
    if (_batchDepth > 0) {
      for (const sub of subscribers) _pendingEffects.push(sub);
    } else {
      for (const sub of [...subscribers]) sub();
    }
  }

  return [get, set];
}

function createEffect(fn) {
  const effect = () => {
    _effectStack.push(_currentEffect);
    _currentEffect = effect;
    try { fn(); } finally { _currentEffect = _effectStack.pop(); }
  };
  effect(); // Run immediately to establish subscriptions
  return effect;
}

function createMemo(fn) {
  const [get, set] = createSignal(undefined);
  createEffect(() => set(fn()));
  return get;
}

function batch(fn) {
  _batchDepth++;
  try {
    fn();
  } finally {
    _batchDepth--;
    if (_batchDepth === 0) {
      const effects = [...new Set(_pendingEffects)];
      _pendingEffects = [];
      for (const effect of effects) effect();
    }
  }
}

function untrack(fn) {
  const prev = _currentEffect;
  _currentEffect = null;
  try { return fn(); } finally { _currentEffect = prev; }
}

// Signals for Counter
const [count, setCount] = createSignal(0);


// Event bindings & reactive DOM updates
document.addEventListener('DOMContentLoaded', () => {
  {
    const el = document.getElementById('Button-5');
    if (el) el.addEventListener('click', (e) => {
      setCount(count() + 1 ); 
    });
  }
  {
    const el = document.getElementById('Button-6');
    if (el) el.addEventListener('click', (e) => {
      setCount(count() - 1 ); 
    });
  }
  {
    const el = document.getElementById('Button-7');
    if (el) el.addEventListener('click', (e) => {
      setCount(0 ); 
    });
  }

  // Fine-grained DOM updates — only update specific text nodes
  createEffect(() => {
    const el = document.getElementById('Text-3');
    if (el) el.textContent = `Count: ${count()}`;
  });
});
