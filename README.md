# Telescope

Telescope is a lightweight database search engine for identifying peptides from tandem mass spectrometry (MS/MS) data. It is built around a fragment ion index, which makes each spectrum search fast enough for real-time search (RTS), where spectra are identified while the instrument is still acquiring them. Telescope serves as a sandbox for developing and testing RTS methods, and it also runs as a conventional command-line search tool over complete data files.

Telescope is developed by the Schweppe Lab and released under the MIT License.

## Components

The repository contains three projects that build from a single Visual Studio solution.

| Project | Type | Purpose |
|---|---|---|
| **Telescope** | C++ console application | The search engine and its command-line interface. It digests a FASTA database, builds the fragment ion index, searches every MS/MS spectrum in one or more data files, and writes the results in pepXML format. |
| **TelescopeSharp** | C++/CLI class library | A thin .NET wrapper around the engine. It exposes index building and single-spectrum searching to C#, so that Telescope can be embedded in .NET applications such as instrument control or RTS software. |
| **TelescopeRTS** | C# Windows Forms application | A demonstration and test bed for RTS. It replays the MS/MS scans of a Thermo `.raw` file at a chosen rate to simulate a live acquisition, searches each scan as it arrives, and reports throughput and timing statistics. Comet can be selected in place of Telescope for side-by-side comparison. |

### Repository layout

```
src/              Telescope engine and command-line sources (C++)
Telescope/        Visual Studio solution and the Telescope command-line project
TelescopeSharp/   TelescopeSharp wrapper project
TelescopeRTS/     TelescopeRTS application project
Telescope.params  Example parameters file for the command-line tool
```

## Requirements

Telescope is developed and tested on 64-bit Windows with Visual Studio 2022. Only the **Release | x64** configuration is maintained.

**Telescope (command line)**

* Visual Studio 2022 with the *Desktop development with C++* workload.
* [MSToolkit](https://github.com/mhoopmann/mstoolkit), used to read spectral data files. By default MSToolkit, and therefore Telescope, is compiled with support for Thermo `.raw` files, which requires Thermo's MSFileReader to be installed on the build machine. To build without it, define `_NO_THERMORAW` when compiling both MSToolkit and Telescope.
* [NeoPepXMLParser](https://github.com/mhoopmann/NeoPepXMLParser), used to write pepXML results. Telescope uses the prebuilt Windows dev kit from the NeoPepXMLParser releases page and links its static library. The kit bundles its own copy of expat, which Telescope does not use: expat comes from MSToolkit, so the program contains exactly one copy.

**TelescopeSharp**

* The *C++/CLI support for v143 build tools* component for Visual Studio, available in the Visual Studio Installer under Individual Components.
* No external libraries. TelescopeSharp compiles the engine sources in `src/` directly and does not use MSToolkit or NeoPepXMLParser.

**TelescopeRTS**

* The *.NET desktop development* workload (.NET 8).
* [Comet](https://github.com/UWPR/Comet). TelescopeRTS references `CometWrapper.dll` from a Comet build.
* The Nova and Nova.IO NuGet packages, which are restored automatically and provide `.raw` file reading.

## Building

### Using the provided solution (recommended)

1. Clone MSToolkit beside this repository. Download the NeoPepXMLParser Windows dev kit (`NeoPepXMLParser-<version>-windows-x64.zip` from its [releases page](https://github.com/mhoopmann/NeoPepXMLParser/releases)) and unpack it into a `Libs` folder beside this repository:

   ```
   mstoolkit\
   Libs\NeoPepXMLParser-<version>-windows-x64\     the unpacked kit, containing include\, lib\, and bin\
   Telescope\
   ```

   The Telescope project looks for MSToolkit there by default and uses the newest NeoPepXMLParser kit it finds in `Libs`, taking the last in name order, which is the highest version as long as the version numbers keep the same number of digits. A kit unpacked directly beside the repository under the name `NeoPepXMLParser` also works. Other locations work too; see step 3.
2. Build MSToolkit for Release, x64, using the same v143 toolset as Telescope: open `VisualStudio\MSToolkit.sln` in its repository and build. Its projects specify the older v142 toolset, so accept Visual Studio's offer to retarget them, or from a command line run `msbuild VisualStudio\MSToolkit.sln -p:Configuration=Release -p:Platform=x64 -p:PlatformToolset=v143`. This produces `MSToolkit.lib` and `MSToolkitExtern.lib` in `mstoolkit\VisualStudio\x64\Release\`.

   NeoPepXMLParser needs no build. The kit ships `NeoPepXMLParser_static.lib` in its `lib` folder, already built with v143 against the dynamic runtime.
3. If the dependencies live somewhere else, tell the Telescope project where to find them. Create a file named `Dependencies.local.props` in the `Telescope\` directory (next to `Telescope.vcxproj`) with the following contents, adjusting the two paths (`NeoPepXMLParserDir` is the root of the unpacked kit, the folder that contains `include` and `lib`):

   ```xml
   <?xml version="1.0" encoding="utf-8"?>
   <Project xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
     <PropertyGroup>
       <MSToolkitDir>C:\path\to\mstoolkit</MSToolkitDir>
       <NeoPepXMLParserDir>C:\path\to\NeoPepXMLParser-1.1.0-windows-x64</NeoPepXMLParserDir>
     </PropertyGroup>
   </Project>
   ```

   This file is ignored by git, so machine-specific paths never end up in the repository. Alternatively, set `MSToolkitDir` and `NeoPepXMLParserDir` as environment variables or pass them to MSBuild with `/p:MSToolkitDir=...`.
4. Open `Telescope\Telescope.sln` in Visual Studio 2022.
5. If you want TelescopeRTS, edit the `HintPath` of the `CometWrapper` reference in `TelescopeRTS\TelescopeRTS.csproj` to point at your `CometWrapper.dll`. If you only need the command-line tool, skip this step and unload the TelescopeRTS project.
6. Select *Release* and *x64* on the toolbar and choose *Build > Build Solution*, or build just the Telescope project.

Build output is written to `Telescope\x64\Release\` for the two C++ projects and to `TelescopeRTS\bin\x64\Release\` for TelescopeRTS.

### Building without the solution

If you are using a different build system, the solution does the following.

* **Telescope** compiles every `.cpp` file in `src/` into one console executable. Add the MSToolkit `include` and `include\extern` directories and the kit's `include` directory to the include path, in that order so that any expat header is taken from MSToolkit. Link against `MSToolkit.lib`, `MSToolkitExtern.lib`, and `NeoPepXMLParser_static.lib`. Do not link the kit's `libexpatMD.lib`; MSToolkitExtern already contains expat. Define `_CRT_SECURE_NO_WARNINGS`, `NDEBUG`, and `NEOPEPXML_STATIC_DEFINE`.
* **TelescopeSharp** compiles `TelescopeSharp\TelescopeSharp.cpp` with common language runtime support (`/clr`) together with these engine sources from `src/`: `DB.cpp`, `DBManager.cpp`, `FastXCorr.cpp`, `FIManager.cpp`, `FIMask.cpp`, `FIMemoryManager.cpp`, `FISpectrum.cpp`, `FragmentIonIndex.cpp`, `ParamsManager.cpp`, `Threading.cpp`, and `TopScore.cpp`. Add `src/` to the include path. The output is a DLL.
* **TelescopeRTS** is a standard .NET SDK project. It references the TelescopeSharp project and `CometWrapper.dll`, and restores its NuGet packages on build.

The threading layer is adapted from Comet and contains both Windows and POSIX code paths, but Telescope has only been built and tested on Windows.

## Usage

### Command line

Telescope takes a single argument, the path to a parameters file:

```
Telescope.exe Telescope.params
```

An example parameters file, `Telescope.params`, is provided at the root of the repository. Copy it, replace the `<path>` placeholders on the `database` and `data_file` lines with your FASTA file and spectral data file, and adjust the remaining values as needed. Running Telescope with no argument prints the version and a usage line.

Telescope prints its progress through each stage (FASTA digestion, index generation, spectrum loading, search, and export) along with timing statistics. For each `data_file` in the parameters file it writes one pepXML result file next to that data file, replacing the data file's extension with `ts.pep.xml`. For example, searching `sample.mzML` produces `sample.ts.pep.xml` in the same directory.

Listing several `data_file` lines searches each file in turn against the same index, so the database is digested and indexed only once.

### Parameters file

The parameters file is plain text with one parameter per line in the form `name = value`. The `=` sign is optional, and anything after `#` is a comment. Only `database` and `data_file` are required. Every other parameter has a default value that applies when the line is omitted.

A minimal example (the full example is `Telescope.params` in the repository root):

```
# Telescope parameters
database = human.fasta
data_file = sample1.mzML
data_file = sample2.mzML

threads = 8
precursor_ppm = 20
psm_per_scan = 5

static_modification = 57.021464 C Carbamidomethyl
variable_modification = 15.994915 3 M Oxidation
```

All parameters:

| Parameter | Default | Description |
|---|---|---|
| `database` | required | FASTA protein database. |
| `data_file` | required | Spectral data file to search. May be repeated. Any format read by MSToolkit is accepted, such as mzML or mzXML. |
| `threads` | 18 | Number of concurrent search threads. |
| `precursor_ppm` | 20 | Precursor mass tolerance in parts per million, applied symmetrically. |
| `psm_per_scan` | 5 | Number of top-scoring peptides written to the pepXML file for each spectrum. |
| `expect` | 0 | Set to 1 to estimate an expectation value (e-value) for each top score. This can increase the search space. This feature is still under active development, so use with caution. |
| `bin_size` | 0.02 | Width in m/z of the bins used to match fragment ions. |
| `min_mz` | 200 | Lowest fragment m/z considered. |
| `max_mz` | 2000 | Highest fragment m/z considered. |
| `max_frag_z` | 3 | Highest fragment ion charge state included in the index. |
| `xcorr` | 1 | Set to 0 to skip the XCorr (cross-correlation) transformation of each spectrum before scoring. |
| `ultra_xcorr` | 0 | Set to 1 to replace the full XCorr transformation with UltraXCorr, a faster approximation of it. |
| `min_peak_xcorr` | 2 | Peaks whose magnitude falls below this value after the XCorr transformation are discarded. |
| `min_peptide_length` | 5 | Shortest peptide, in residues, kept from the digest. |
| `max_peptide_length` | 50 | Longest peptide, in residues, kept from the digest. |
| `min_peptide_mass` | 600 | Lowest peptide mass, in Daltons, kept from the digest. |
| `max_peptide_mass` | 5000 | Highest peptide mass, in Daltons, kept from the digest. |
| `max_missed_cleavage` | 2 | Maximum number of missed enzymatic cleavages per peptide. |
| `enzyme_specificity` | 0 | 0 requires both peptide termini to be enzymatic. 1 allows semi-specific peptides, where only one terminus is enzymatic. |
| `max_peptide_modification` | 2 | Maximum number of variable modifications on a single peptide. |
| `static_modification` | none | `mass sites description`. A fixed mass added to every occurrence of the listed residues. May be repeated. |
| `variable_modification` | none | `mass max_per_peptide sites description`. An optional mass considered on the listed residues. May be repeated. |

For modifications, `sites` is a list of one-letter amino acid codes with no separators, for example `STY`. The letters `n` and `c` denote the peptide N-terminus and C-terminus. The `description` is a single word used to label the modification in the output.

Digestion is currently fixed to trypsin, cleaving after K and R except when followed by P.

### Using TelescopeSharp from C#

TelescopeSharp exposes two classes in the `TelescopeSharp` namespace. `Telescope` holds the database and index. `TScore` carries the result of one spectrum search.

```csharp
using TelescopeSharp;

var engine = new Telescope();

// Reads the parameters file, digests the FASTA database, and builds the index.
// This can take from seconds to several minutes depending on the database.
if (!engine.Init("telescope.params"))
{
    throw new Exception("Telescope failed to initialize.");
}

// mz and intensity hold the peaks of one MS/MS spectrum, ordered by increasing m/z.
// The first argument selects a per-thread scratch buffer (see below).
TScore hit = engine.Search(0, mz, intensity, precursorMz, precursorCharge);

if (hit.score > 0)
{
    Console.WriteLine($"{hit.peptide}\t{hit.protein}\t{hit.score}");
}
```

Points to note:

* `Init` accepts the same parameters file as the command-line tool. Any `data_file` lines are ignored, because spectra are supplied through `Search`.
* The first argument to `Search` is a thread index from 0 to `threads - 1`, where `threads` is the value in the parameters file. Each index selects a private scratch buffer, so calls running at the same time must use different indexes.
* A `precursorCharge` of 0 is treated as 3.
* A returned `score` of 0 means no peptide matched, or the precursor mass fell outside the `min_peptide_mass` to `max_peptide_mass` range.
* `GetPeptidoformCount()` and `GetMemUse(bool)` report the size of the loaded search space. Passing `true` to `GetMemUse` returns the memory used by the peptide list in gigabytes; `false` returns the memory used by the index.

### TelescopeRTS

TelescopeRTS walks through a fixed sequence of steps, enabling each control as the previous step completes.

1. Choose *Telescope* or *Comet* in the search engine list and click *Lock Algorithm*.
2. Select a parameters file for the chosen engine. Telescope builds its index at this point, which can take several minutes for a large database.
3. Select a Thermo `.raw` file. Its MS2 scans are loaded into memory. Scans with fewer than 25 peaks, no precursor charge, or a precursor mass outside 800 to 4000 Da are skipped.
4. Set the scan rate in scans per second (default 20) to control how quickly scans are streamed. For Telescope, also set the number of search threads (1 to 20). Comet always searches on a single thread.
5. Click *Run*. Scans are streamed at the chosen rate and searched as they arrive. The window reports the number of scans processed and matched, average search and lag times, fastest and slowest searches, active threads, and the number of scans waiting in the queue. Click *Stop* to end a run early.
6. Click *Export Log* to save the run log and the search results. Click *Unlock* to release the engine and return to the first step.

The `.raw` file can be reloaded between runs without repeating the index build.

## License

Telescope is released under the MIT License. See [LICENSE](LICENSE).

Several source files incorporated from other projects carry their own Apache License 2.0 headers, which are preserved in those files. These include the threading layer adapted from Comet (`Threading.h`, `Threading.cpp`, `ThreadPool.h`, and `OSSpecificThreading.h`) and the FASTA parser (`DB.h` and `DB.cpp`).
