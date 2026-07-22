# Case Study: File Integrity Monitor

**Problem.** Detecting unauthorized changes to critical files is a fundamental security requirement. Manual checks are impractical for large directories, and the time between a malicious modification and its discovery can be days or weeks.

**Solution.** File Integrity Monitor calculates SHA-256 hashes of every file in a directory and stores them as a reference baseline. A verification mode recalculates current hashes and compares them against the baseline, clearly reporting any modified, new, or missing files.

**Result.** The tool provides a lightweight way to audit file integrity, ideal for configuration directories, web roots, or any location where unexpected changes should trigger an immediate alert. Its automated test suite (Google Test + CI/CD) ensures the detection logic remains reliable as the project evolves.
