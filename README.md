## 🛠️ Game Hacking: Memory Analysis and Interface Development

This project documents the process of **reverse engineering** and the subsequent development of a modification *framework*, initially focused on implementing an **infinite ammunition** feature.

---

### 🎯 Objective

The main goal was to identify and manipulate the memory address responsible for the player's ammunition counter. This was achieved by employing scanning techniques to locate a **static pointer** through repeated analysis of a **"volatile" base pointer** (also known as a *dynamic* or *floating address*).

---
<img width="1362" height="530" alt="image" src="https://github.com/user-attachments/assets/5f38d42b-7bb2-4cfc-b397-5a103f76054f" />

### 🧠 Reverse Engineering Methodology

Identifying the static memory address was a multi-step process involving continuous observation and comparison of memory values:

1.  **Initial Scan:** Using memory analysis software to identify the **volatile (dynamic) address** that held the current ammunition value.
2.  **Pointer Tracing:** Performing successive **pointer comparisons** (chain scanning) to trace the path from the volatile address back to its base address—the **static pointer** (the address that remains constant across game sessions) which referenced it.
3.  **Static Identification:** Once the static pointer was located, it became the target for direct manipulation of the ammunition value.

---

### 💻 Interface and Feature Development

With the ammunition pointer successfully identified, the development of the Graphical User Interface (GUI) and the modification logic began.

#### **Technologies Used:**

* **Graphical User Interface (GUI):** **Dear ImGui** (Immediate Mode Graphical User Interface), chosen for its lightweight nature and ease of integration.
* **Hooking:** **MinHook**, utilized for creating **hooks** into game functions and injecting the modification code.

#### **Current Feature:**

* **Infinite Ammunition:** The currently implemented feature continuously reads and writes to (or intercepts the decrement of) the static address found, ensuring the ammunition value remains maximum or prevents its depletion.

#### **Next Steps (Roadmap):**

The immediate focus is the completion of the **DLL Injector** to simplify the application of the *framework* to the target game. Subsequently, development will focus on adding new functionalities, such as:

* Infinite health.
* Speed modifications (Speedhack).
* Teleportation.
