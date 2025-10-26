const express = require('express');
const { execFile } = require('child_process');
const path = require('path');

const app = express();
const port = process.env.PORT || 3000;

// GET /search?q=some+query
app.get('/search', (req, res) => {
  const q = String(req.query.q || '').trim();
  if (!q) return res.status(400).json({ error: 'Missing q' });

  const binPath = path.join(__dirname, 'bin', 'sourcefinder'); // your compiled C++ program

  // Use execFile to avoid shell injection
  execFile(binPath, [q], { timeout: 15000, maxBuffer: 5 * 1024 * 1024 }, (err, stdout, stderr) => {
    if (err) {
      console.error('C++/Python error:', err, stderr);
      return res.status(502).json({ error: 'Upstream search failed' });
    }
    try {
      const data = JSON.parse(stdout);
      return res.json(data);
    } catch (e) {
      console.error('Bad JSON from binary:', e, 'stdout:', stdout);
      return res.status(500).json({ error: 'Invalid response format' });
    }
  });
});

app.listen(port, () => {
  console.log(`API listening on http://localhost:${port}`);
});