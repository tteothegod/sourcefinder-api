// server.js
const express = require('express');
const { execFile } = require('child_process');
const app = express();
const port = 8080;

app.get('/api/search', (req, res) => {
  const query = req.query.query || '';
  execFile('./DataPrep', [query], (err, stdout, stderr) => {
    if (err) return res.status(500).send(stderr || err.message);
    try {
      const obj = JSON.parse(stdout);
      res.json(obj);
    } catch (parseErr) {
      res.status(500).send('Invalid JSON from C++ binary');
    }
  });
});

app.listen(port, () => {
  console.log(`Server listening at http://localhost:${port}`);
});
