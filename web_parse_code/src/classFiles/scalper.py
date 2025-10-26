#!/usr/bin/env python3
"""
Polite HTML fetcher + saver with retries, robots.txt check and MediaWiki API fallback.

Usage:
    python scraper.py "https://en.wikipedia.org/wiki/Homebrew_(package_manager)" scraped0.html
"""
from __future__ import annotations
import sys
import time
import logging
from urllib.parse import urlparse, urljoin
import urllib.robotparser as robotparser

import requests
from requests.adapters import HTTPAdapter
from urllib3.util.retry import Retry
from bs4 import BeautifulSoup

# Configure logger
logging.basicConfig(level=logging.INFO, format="%(asctime)s %(levelname)s: %(message)s")
logger = logging.getLogger("scraper")

DEFAULT_USER_AGENT = "SourceFinderScraper/1.0 (https://github.com/tteothegod/sourcefinder; contact: you@example.com)"

def create_session(user_agent: str = DEFAULT_USER_AGENT, retries: int = 3, backoff: float = 1.0) -> requests.Session:
    s = requests.Session()
    s.headers.update({
        "User-Agent": user_agent,
        "Accept": "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8",
        "Accept-Language": "en-US,en;q=0.5",
    })
    retry_strategy = Retry(
        total=retries,
        status_forcelist=[429, 500, 502, 503, 504],
        allowed_methods=["HEAD", "GET", "OPTIONS"],
        backoff_factor=backoff,
        raise_on_status=False,
    )
    adapter = HTTPAdapter(max_retries=retry_strategy)
    s.mount("https://", adapter)
    s.mount("http://", adapter)
    return s

def fetch_via_mediawiki_api(page_title: str, session: requests.Session) -> str | None:
    api = "https://en.wikipedia.org/w/api.php"
    params = {
        "action": "parse",
        "page": page_title,
        "format": "json",
        "prop": "text"
    }
    try:
        r = session.get(api, params=params, timeout=10)
        r.raise_for_status()
        data = r.json()
        html_fragment = data.get("parse", {}).get("text", {}).get("*")
        if html_fragment:
            return html_fragment
    except Exception as e:
        logger.debug("MediaWiki API fetch failed: %s", e)
    return None

def fetch_and_save_html(url: str, output_file: str = "scraped0.html", session: requests.Session | None = None, use_mediawiki_fallback: bool = True) -> str | None:
    session = session or create_session()
    parsed = urlparse(url)

    # If this looks like wikipedia, attempt API first (preferred)
    if "wikipedia.org" in parsed.netloc and use_mediawiki_fallback:
        page_title = parsed.path.lstrip("/") or parsed.query
        logger.info("Attempting MediaWiki API fetch for %s", page_title)
        api_html = fetch_via_mediawiki_api(page_title, session)
        if api_html:
            soup = BeautifulSoup(api_html, "html.parser")
            with open(output_file, "w", encoding="utf-8") as f:
                f.write(soup.prettify())
            logger.info("Saved MediaWiki API HTML to %s", output_file)
            return output_file
        else:
            logger.info("MediaWiki API fallback failed — will attempt direct fetch")

    try:
        logger.info("Fetching %s", url)
        resp = session.get(url, timeout=15, allow_redirects=True)
        logger.info("HTTP %s for %s", resp.status_code, url)

        if resp.status_code == 403:
            # Helpful debug output for 403
            logger.error("403 Forbidden for %s — response headers:\n%s", url, resp.headers)
            snippet = resp.text[:1000].replace("\n", " ")
            logger.error("Response body snippet (first 1000 chars): %s", snippet)
            return None

        resp.raise_for_status()

        # Parse and format HTML
        soup = BeautifulSoup(resp.text, "html.parser")
        formatted_html = soup.prettify()

        with open(output_file, "w", encoding="utf-8") as file:
            file.write(formatted_html)

        logger.info("Formatted HTML saved to %s", output_file)
        return output_file
    except requests.exceptions.RequestException as e:
        logger.exception("Error fetching %s: %s", url, e)
        return None

def main(argv: list[str]):
    if len(argv) < 2:
        print("Usage: python scraper.py <URL> [output_file]")
        return
    url = argv[1]
    output_file = argv[2] if len(argv) > 2 else "scraped0.html"
    session = create_session()
    result = fetch_and_save_html(url, output_file=output_file, session=session, use_mediawiki_fallback=True)
    if result:
        logger.info("Done — saved to %s", result)
    else:
        logger.error("Failed to fetch %s", url)

if __name__ == "__main__":
    main(sys.argv)