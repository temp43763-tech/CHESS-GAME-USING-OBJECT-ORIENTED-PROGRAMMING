

# Work in progress — movement logic implemented for King, Rook, Bishop, Queen and Pawn. Game loop not yet complete.

# Code Review Report: `chessoops.cpp`
**Project:** Chess Game (C++ OOP Implementation)  
**File Reviewed:** `chessoops.cpp`  
**Language:** C++  
**Date:** April 25, 2026  

---

## 1. Executive Summary

This file implements the beginnings of a chess game using object-oriented C++. It demonstrates a solid grasp of core OOP principles — inheritance, polymorphism, and abstract classes — and correctly models the chess board and several piece types. However, the project is incomplete: the `main()` function is empty, meaning the game cannot be played. There are also several critical bugs (null pointer dereferences, unreachable code) and notable missing features (Knight, turn management, check detection). Overall, this represents **beginner-to-intermediate level** work, consistent with a university-level OOP assignment.

---

## 2. Project Structure Overview

| Component | Status |
|---|---|
| `piece` (abstract base class) | ✅ Implemented |
| `Chessboard` | ✅ Implemented (partial) |
| `King` | ✅ Implemented |
| `Rook` | ✅ Implemented (with bug) |
| `Bishop` | ✅ Implemented (with bug) |
| `Queen` | ✅ Implemented (via delegation) |
| `Pawn` | ⚠️ Partially implemented (promotion logic broken) |
| `Knight` | ❌ Missing |
| `main()` / Game Loop | ❌ Empty |
| Turn Management | ❌ Missing |
| Check / Checkmate Detection | ❌ Missing |
| Input Parsing | ❌ Missing |

---

## 3. Strengths

### 3.1 Good Use of OOP
The design correctly uses an abstract base class (`piece`) with a pure virtual function (`movementlogic`), enforcing that every piece must define its own movement. Polymorphism is used properly: the board stores `piece*` pointers, and the correct `movementlogic` is dispatched at runtime. This is exactly the right approach for a chess engine.

### 3.2 Reasonable Class Separation
`Chessboard` and `piece` are well separated. The board handles storage and display, while each piece handles its own movement validation. This separation of concerns is good design practice.

### 3.3 Movement Logic is Mostly Sound
The core movement rules for King, Rook, and Bishop are logically correct. The Rook correctly iterates over intermediate squares to detect blocking pieces. The Bishop correctly checks the diagonal path step-by-step. The King correctly limits movement to one square in any direction.

### 3.4 Pawn Direction Awareness
The pawn correctly uses a `rowstep` variable (`-1` for white, `+1` for black) to handle the directional difference between the two sides, and correctly identifies the starting row for the two-square advance.

---

## 4. Bugs and Critical Issues

### 4.1 🔴 Null Pointer Dereference in `Rook::movementlogic`
**Severity: Critical**

```cpp
piece *destPiece = obj.ispieceat(desrow, descol);
if ((iswhite() && isupper(destPiece->getchar())) || ...)
```

`destPiece` is never checked for `nullptr` before calling `->getchar()`. If the destination square is empty, this causes undefined behavior (likely a crash). The same bug exists in both the horizontal and vertical movement branches.

**Fix:** Add a null check before dereferencing:
```cpp
if (destPiece != nullptr && (...))
```

---

### 4.2 🔴 Same Null Pointer Bug in `Bishop::movementlogic`
**Severity: Critical**

Identical issue — `destPiece` is dereferenced without a null check. Moving a Bishop to an empty square will crash the program.

---

### 4.3 🔴 Broken Pawn Promotion Logic
**Severity: Critical**

The promotion block in `pawn::movementlogic` is placed **outside** all the `if` branches, meaning it runs unconditionally on every call, regardless of whether the move was valid or not:

```cpp
// This runs even if none of the above conditions matched:
delete obj.getPieceAt(desrow, descol);
if (iswhite() && desrow == 0 || isblack() && desrow == 7)
{ ... }
```

This will delete pieces at arbitrary board positions and corrupt the board state. The promotion logic should only trigger after a successful one-square forward move to the back rank.

---

### 4.4 🟡 Queen's Delegation Creates a Temporary Object That Moves the Piece Itself
**Severity: Moderate**

```cpp
Rook r(color);
Bishop b(color);
if (b.movementlogic(scrrow, scrcol, desrow, descol, obj)) { return true; }
```

The temporary `Rook` and `Bishop` objects call `obj.move()` inside their own `movementlogic`, which means the piece is actually moved by the temporary object. This works accidentally, but it's a fragile and confusing design. If the Bishop validates the move and calls `obj.move()`, the Queen's own `movementlogic` returns `true` without ever knowing a move was made — the `Queen` object itself was never placed.

---

### 4.5 🟡 `Chessboard::move()` Skips Validation
**Severity: Moderate**

```cpp
bool move(int scrrows, int scrcol, int desrow, int descol)
{
    if (board[scrrows][scrcol] == nullptr) return false;
    delete board[desrow][descol];
    board[desrow][descol] = board[scrrows][scrcol];
    board[scrrows][scrcol] = nullptr;
    return true;
}
```

The board's `move()` method performs no bounds checking and does not call `movementlogic`. It's possible to call this directly and bypass all movement rules. Bounds checking (`row >= 0 && row < 8`) should be added.

---

### 4.6 🟡 Duplicate Condition in Pawn
**Severity: Minor**

```cpp
if (scrcol == descol && scrrow == startrow && desrow == scrrow + 2 * rowstep
    && destination == nullptr && destination == nullptr)  // duplicated condition
```

`destination == nullptr` is checked twice. This is a copy-paste oversight and should also check that the intermediate square is empty (to prevent jumping over pieces on the two-square advance).

---

### 4.7 🟡 Operator Precedence Issue
**Severity: Minor**

In several places, `&&` and `||` are mixed without parentheses:
```cpp
if (iswhite() && isupper(despostion->getchar()) || (isblack() && islower(despostion->getchar())))
```

Since `&&` binds tighter than `||`, this is equivalent to:
```cpp
if ((iswhite() && isupper(...)) || (isblack() && islower(...)))
```

This happens to be logically correct here, but it's poor style and a source of future bugs. Always use explicit parentheses when mixing `&&` and `||`.

---

## 5. Missing Features

### 5.1 `main()` is Empty
The game cannot be run. There is no game loop, no board initialization with pieces, and no user input handling. This is the most significant gap — without `main()`, this is a library of classes, not a game.

### 5.2 No `Knight` Class
The Knight is referenced in a comment inside the pawn's promotion logic (`// board[desrow][descol] = new Knight(pawn_color);`) but was never implemented. The Knight's L-shaped movement is straightforward to implement.

### 5.3 No Input Parsing
There is no function to convert standard chess notation (e.g., `"e2 e4"`) to board array indices. This is necessary for a functional game.

### 5.4 No Turn Management
There is no mechanism to track whose turn it is or to enforce alternating play between white and black.

### 5.5 No Check or Checkmate Detection
The game has no awareness of whether a king is in check, which is a fundamental rule of chess. Without this, illegal moves that leave a king in check can be made freely.

### 5.6 No En Passant or Castling
These are special rules not expected at this level of implementation, but worth noting as future work.

---

## 6. Memory Management Concerns

The project uses raw `new` and `delete` throughout, which introduces risk:

- **No destructor in `Chessboard`:** The board's destructor never deletes the pieces it holds, causing a memory leak when the board goes out of scope.
- **Double-free risk:** The broken pawn promotion code calls `delete obj.getPieceAt(desrow, descol)` twice in some paths.
- **Recommendation:** Consider using `std::unique_ptr<piece>` for board squares to automate ownership and deletion.

---

## 7. Code Quality and Style

| Criterion | Assessment |
|---|---|
| Naming conventions | Inconsistent (mix of camelCase, lowercase, PascalCase) |
| Comments | Sparse but present in key areas |
| Error messages | Present but inconsistent (some use `cout`, some `cerr`) |
| Code duplication | Moderate (Rook's two branches are near-identical) |
| Header usage | Minimal and appropriate for scope |

---

## 8. Recommendations (Priority Order)

1. **Fix the null pointer bugs** in `Rook` and `Bishop` before anything else.
2. **Fix the pawn promotion block** — move it inside the one-step forward move condition and only trigger at the back rank.
3. **Implement `main()`** with board setup, a game loop, and input parsing.
4. **Add `Knight` class.**
5. **Add bounds checking** to `Chessboard::move()`.
6. **Add turn management** to enforce alternating play.
7. **Refactor `Queen`** to avoid moving the piece via a temporary object.
8. **Add a `Chessboard` destructor** to prevent memory leaks.
9. **Consider smart pointers** (`unique_ptr`) to simplify memory management.
10. **Implement check detection** as the final correctness milestone.

---

## 9. Conclusion

This is a promising start. The OOP architecture is sound, and the movement logic for most pieces is close to correct. With the critical bugs fixed and `main()` implemented, this could become a functional (if basic) playable chess game. The next major milestone should be getting a real game loop running — at which point many of the remaining issues will become apparent through playtesting.

**Overall Rating: 5.5 / 10** — Good foundation, not yet functional.
