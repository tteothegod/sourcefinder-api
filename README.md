# Source Finder — API & Scraper (sourcefinder-api)

Repository: https://github.com/tteothegod/sourcefinder-api

This repository contains the scraping, parsing, and API/C++ code that powers Source Finder's backend. The goal is to fetch HTML from web pages, extract candidate academic sources (citations, DOIs, metadata) using Python parser classes, and expose an API + model that can rank and serve results to the frontend (https://tteothegod.github.io/sourcefinder/).

Live frontend: https://tteothegod.github.io/sourcefinder/

## Key ideas
- Scrapers: Python modules (in web_parse_code/src/classFiles/) that fetch HTML and parse candidate academic citations using custom parser classes.
- C++ runner: a C++ executable (Scraper) that embeds Python (Python/C API) to call the Python scrapers and save the parsed HTML/output. The binary in this repo was compiled for macOS.
- API & model: A neural ranking model is being trained; an API will be implemented to serve the model and orchestrate scrapers and ranking.
- Integration: The frontend queries the backend API (when implemented) to get ranked candidate academic sources.

## Status
- Python scraping/parsing classes: implemented under web_parse_code/src/classFiles (parsers and helper scripts).
- C++ runner (Scraper): available and compiled for macOS. It calls the Python scraper functions and writes output HTML files.
- Neural network & API: model training in progress; API endpoints and inference server under development.

## Requirements (tested on macOS)
- macOS (binary compiled for mac)
- Python 3.8+ (3.x) — the Python used to build/run scrapers
- pip (or pip3) to install Python dependencies
- Python packages (install with pip): requests, beautifulsoup4
  - Example: pip3 install -r web_parse_code/requirements.txt
  - If the repo does not include requirements.txt, install manually:
    pip3 install requests beautifulsoup4
- If you build from source: CMake, a C++17-capable compiler, and Python development headers (brew install cmake python)
- Note: The provided Scraper executable links to a macOS Python runtime; if you build locally, ensure your Python dev headers and library are available.

## Quick start — run the Scraper (prebuilt binary)
1. Clone the repo:
   git clone https://github.com/tteothegod/sourcefinder-api.git
2. Change into the web_parse_code directory (this is important — the Scraper binary expects to be run from this folder):
   cd sourcefinder-api/web_parse_code
3. Install Python deps (if not already installed):
   pip3 install -r requirements.txt
   or
   pip3 install requests beautifulsoup4
4. Make sure the Scraper executable is present and executable. If the repo provides a prebuilt binary named `Scraper`, ensure it has execute permissions:
   chmod +x Scraper
5. Run the Scraper:
   ./Scraper "https://en.wikipedia.org/wiki/Homebrew_(package_manager)" scraped0.html

Notes:
- The Scraper executable was compiled for macOS. If you're on macOS and running into dynamic library/Python linking issues, ensure your system Python or Homebrew Python is compatible with the binary.
- The current design expects the process to be started from the `web_parse_code` directory so relative paths to `src/classFiles` resolve correctly. If you run the binary from elsewhere, it may not locate the Python modules.

## Building from source (optional)
If you want to compile the C++ code yourself (macOS / Linux):
1. Install prerequisites:
   - Homebrew (macOS) or your platform's package manager
   - cmake, a C++ compiler with C++17 support
   - Python and Python development headers (e.g., brew install python)
2. Build:
   mkdir -p build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j$(nproc)
3. The resulting executable (e.g., `Scraper`) will be in the build directory — copy or run it from `web_parse_code` (or ensure it can find `src/classFiles` by setting PYTHONPATH or running from repo root).

## Usage examples
- Single URL scrape:
  ./Scraper "https://en.wikipedia.org/wiki/Soda_Stereo" scraped1.html
- Multiple runs: the Scraper generates filenames like `scraped0.html`, `scraped1.html`, etc.
- The Python scraper function `fetch_and_save_html(url, output_file)` receives the URL and output filename; the C++ executable calls it via the Python/C API.

## Design & implementation notes
- The C++ runner embeds Python and manipulates Python's sys.path so the `src/classFiles` Python modules can be imported. Currently the executable expects to be run from the `web_parse_code` directory so relative import paths work (this is intentional for the current setup).
- For Wikipedia pages, prefer using the MediaWiki API rather than scraping the HTML directly; the scraper includes a MediaWiki API fallback for Wikipedia URLs.
- Be polite when scraping:
  - Set a descriptive User-Agent identifying the tool and a contact email (e.g., SourceFinderScraper/1.0 (https://github.com/tteothegod/sourcefinder-api; contact: you@example.com)).
  - Respect robots.txt and rate limits.
  - Use caching, delay between requests, and avoid high request rates.

## Troubleshooting
- 403 Forbidden / blocked responses:
  - Many sites block default Python User-Agent strings. Ensure the scraper sets a custom User-Agent and respects robots.txt.
  - For Wikipedia, use the MediaWiki API when possible (fewer blocks and is the recommended approach).
- Cannot import Python module:
  - Make sure you run the Scraper from `web_parse_code`, and `web_parse_code/src/classFiles` exists and contains the Python .py files.
  - Alternatively set PYTHONPATH to include the absolute path to `web_parse_code/src/classFiles` before running:
    export PYTHONPATH="$(pwd)/src/classFiles:$PYTHONPATH"
- macOS binary linking errors:
  - Install a compatible Python runtime (Homebrew python) and ensure library paths match the one used for linking. If needed, rebuild the executable from source with your local Python.

## Contributing
Contributions are welcome. Suggested workflows:
- Bug reports & issues: open an issue in this repo with reproduction steps.
- Pull requests: fork, create a feature branch, add tests and documentation, and open a PR.
- For scraper/parser changes: please include sample URLs and expected output snippets.

## Roadmap
- Finalize API endpoints for search and inference (coming soon).
- Finish neural network training and integrate ranking in API.
- Add comprehensive unit & integration tests for parsers and the C++ -> Python interface.

## License
Add a LICENSE file to specify the project license (for example, MIT).

## Contact
Author: tteothegod  
Email: matteogodinez2024@gmail.com

Frontend repo: https://github.com/tteothegod/sourcefinder
This repo (API & scrapers): https://github.com/tteothegod/sourcefinder-api