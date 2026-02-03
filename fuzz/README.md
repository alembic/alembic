# Alembic Fuzzing

This directory contains fuzzing harnesses for Alembic as part of the OSS-Fuzz continuous fuzzing initiative.

## Fuzzers

### alembic_getarchive_vector_fuzzer.cc

Tests the `Alembic::AbcCoreFactory::v12::IFactory::getArchive(std::vector<std::string> const&)` function, which handles multi-file archive loading.

#### Input Format
The fuzzer expects input in the following format:
```
[4-byte little-endian length][file data][4-byte length][file data]...
```

#### Security Benefits
- Tests vector-based archive loading (previously untested)
- Handles multi-file archive layering functionality
- Includes memory safety limits and exception handling

#### Corpus
Located in `corpus/alembic_getarchive_vector_fuzzer/` with seed inputs for various scenarios.

## OSS-Fuzz Integration

These fuzzers are designed to run continuously on Google's OSS-Fuzz infrastructure, providing:
- Automated vulnerability detection
- Continuous security testing
- Private vulnerability reporting

## Building

The fuzzers are built using the OSS-Fuzz build system. For local testing:

```bash
# Clone OSS-Fuzz and build Alembic fuzzers
git clone https://github.com/google/oss-fuzz.git
cd oss-fuzz
python infra/helper.py build_fuzzers alembic
```

## Contributing

When adding new fuzzers:
1. Follow the naming convention: `alembic_[function]_fuzzer.cc`
2. Include proper Apache 2.0 license header
3. Add seed corpus in `corpus/[fuzzer_name]/`
4. Include dictionary if applicable
5. Test locally before submission

## Security

For security issues found by these fuzzers, please follow Alembic's security policy or report privately through OSS-Fuzz.
