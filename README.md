# Source Finder — API & Scraper (sourcefinder-api)

Repository: https://github.com/tteothegod/sourcefinder-api

This repository contains the scraping, parsing, and API/C++ code that powers Source Finder's backend. The goal is to fetch HTML from web pages, extract candidate academic sources (citations, DOIs, metadata) using Python parser classes, and expose an API + model that can rank and serve results to the frontend (https://tteothegod.github.io/sourcefinder/).

Live frontend: https://tteothegod.github.io/sourcefinder/

## Key ideas
- Scrapers: Python modules (in web_parse_code/src/classFiles/) that fetch HTML.
- C++ runner: a C++ executable (Scraper) that embeds Python (Python/C API) to call the Python scrapers and parse candidate academic citations using custom parser classes in C++. The binary in this repo was compiled for macOS.
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
   ./Scraper

Notes:
- The Scraper executable was compiled for macOS. If you're on macOS and running into dynamic library/Python linking issues, ensure your system Python or Homebrew Python is compatible with the binary.
- The current design expects the process to be started from the `web_parse_code` directory so relative paths to `src/classFiles` resolve correctly. If you run the binary from elsewhere, it may not locate the Python modules.

## Roadmap
- Finalize API endpoints for search and inference (coming soon).
- Finish neural network training and integrate ranking in API.
- Add comprehensive unit & integration tests for parsers and the C++ -> Python interface.

## Contact
Author: tteothegod  
Email: matteogodinez2024@gmail.com

Frontend repo: https://github.com/tteothegod/sourcefinder
This repo (API & scrapers): https://github.com/tteothegod/sourcefinder-api
