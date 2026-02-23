# HW2 – Problem 3  
## Formal Verification of the Lustre Switch using LUKE

This problem converts the Lustre switch from Problem 2 into a **formal verifier** and checks properties using the **LUKE model checker**.

Required deliverables:
```
verify1.lus
verify2.lus
verify3.lus
README2.md
```
No Makefile is required.

---

# 1. Objective

We verify the switch automaton against three required properties:

| File | Property being checked |
|---|---|
| verify1.lus | Either the light is OFF, or x is positive |
| verify2.lus | Either the light is ON, or x is zero |
| verify3.lus | x does not exceed the value 9 |

Verification is performed using **bounded model checking** with LUKE.

---

# 2. Running the Luke Model Checker
---

# 2.1 Run All Three Verifications (Required Commands + Output Interpretation)

This section runs **all three required LUKE checks** and explains how to interpret the results.

## A) Run each verification (basic + bounded int mode)

1) Verify1:
```bash
luke --help
luke verify1.lus --verify --node verify1 --verbose
luke verify1.lus --verify --node verify1 --verbose --int 4
```

2. Verify2:
```bash
luke verify2.lus --verify --node verify2 --verbose
luke verify2.lus --verify --node verify2 --verbose --int 4
```

3. Verify3:
```bash
luke verify3.lus --verify --node verify3 --verbose
luke verify3.lus --verify --node verify3 --verbose --int 4
```

---

## B) What to look for in LUKE output

LUKE reports one of two outcomes:

### 1) Property HOLDS (proof within the explored bounds)

You will see wording indicating the property is **valid / holds**.

In this case:

* No counterexample is generated
* The property is considered proven (within the selected integer bound if `--int N` is used)

### 2) Property FAILS (disproof)

You will see wording indicating the property is **violated / falsified**, and LUKE will generate a counterexample trace.

When a counterexample is produced, LUKE writes an HTML file under the `Luke/` folder (per the assignment examples).

---

## C) Open the counterexample trace (when a property fails)

If `verify1` fails:

```bash
google-chrome --allow-file-access-from-files Luke/verify1.html
```

If `verify2` fails (expected not to):

```bash
google-chrome --allow-file-access-from-files Luke/verify2.html
```

If `verify3` fails:

```bash
google-chrome --allow-file-access-from-files Luke/verify3.html
```

These HTML traces show a concrete execution path (inputs and internal signals) that violates the property.

---

## D) Run all three in one command block

The following runs all three verifications sequentially (useful for re-testing after edits):

```bash
luke verify1.lus --verify --node verify1 --verbose --int 4
luke verify2.lus --verify --node verify2 --verbose --int 4
luke verify3.lus --verify --node verify3 --verbose --int 4
```

If any property fails, open the corresponding HTML trace under `Luke/`.


---



# 6. Observations, Correctness, and Tool Limitations

This section summarizes what was learned from running the Luke model checker and interpreting the generated counterexamples.

---

## 6.1 Sample Verification Runs

All properties were checked using bounded integer reasoning:

```bash
luke verify1.lus --verify --node verify1 --verbose --int 4
luke verify2.lus --verify --node verify2 --verbose --int 4
luke verify3.lus --verify --node verify3 --verbose --int 4
```

Example output for a failing property:

```text
==> Performing Induction ...
* Step 0: output 'ok' ... disproved
> Base 1: output 'ok' ... disproved
*** CHECK FAILED ("verify1.lus"):
Falsified output 'ok' in node 'verify1' at depth 1.
Counter-example generated in file 'Luke/verify1.html'.
```

Luke automatically generates an interactive counterexample trace when a property fails. 

These traces can be opened with:

```bash
google-chrome --allow-file-access-from-files Luke/verify1.html
google-chrome --allow-file-access-from-files Luke/verify3.html
```

---

## 6.2 Interpretation of Counterexamples

### Property 1

**Either the light is OFF, or x is positive**

Result: **False**

Luke finds a reachable state where:

```
mode = ON
x = 0
```

This occurs immediately after the light turns on.
The system initializes the counter to zero at activation, so the property is not valid.

This counterexample is visible in the generated trace interface. 

---

### Property 2

**Either the light is ON, or x is zero**

Result: **True**

Luke successfully proves the property.

When the light turns OFF, the counter resets to zero.
No counterexample exists.

---

### Property 3

**x does not exceed the value 9**

Result: **False**

Luke produces a counterexample showing:

```
x = 10
```

This happens because the system turns OFF *after* the counter reaches 10, not before.
The trace confirms the counter reaches 10 during normal operation. 

---

## 6.3 Correctness Discussion

The verification results match the intended behavior of the switch automaton:

| Property         | Result | Reason                               |
| ---------------- | ------ | ------------------------------------ |
| Light OFF or x>0 | False  | System allows ON with x=0 at startup |
| Light ON or x=0  | True   | Counter resets whenever OFF          |
| x ≤ 9            | False  | Counter reaches 10 before shutdown   |

The model checker therefore correctly identifies both valid and invalid system properties.

---

## 6.4 Tool Limitations

While Luke is effective, several limitations were observed:

* Integer reasoning requires explicit bounds (`--int N`)
* Results depend on the chosen integer bound
* Counterexamples must be viewed in a browser
* GUI trace viewing requires local file access permissions
* Setup requires manual PATH configuration on ARC

Despite these limitations, Luke successfully demonstrated both proof and disproof of the required properties.

---

## 6.5 Deliverables Checklist

The following files are submitted as required:

```
verify1.lus
verify2.lus
verify3.lus
README2.md
```
No Makefile is included, as specified in the assignment.
