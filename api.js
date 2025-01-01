const express = require('express');
const cors = require('cors');
const mongoose = require('mongoose');
const dotenv = require('dotenv');
const app = express();
const port = 80;

dotenv.config();

app.use(cors());
app.use(express.json());

mongoose.connect(process.env.MONGO_URI);

const sensorDataSchema = new mongoose.Schema({
  flameValue: Number,
  smokeValue: Number,
  pumpStatus: Boolean,
  timestamp: { type: Date, default: Date.now }
});

const SensorData = mongoose.model('SensorData', sensorDataSchema);

app.get('/api/sensor-data/latest', async (req, res) => {
  try {
    const latestData = await SensorData.findOne().sort({ timestamp: -1 });
    res.json(latestData);
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

app.get('/api/sensor-data/history', async (req, res) => {
  try {
    const history = await SensorData.find().sort({ timestamp: -1 }).limit(100);
    res.json(history);
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

app.post('/api/sensor-data', async (req, res) => {
  try {
    const sensorData = new SensorData(req.body);
    await sensorData.save();
    res.status(201).json(sensorData);
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

app.get('/api/pump/status', async (req, res) => {
  try {
    const latestData = await SensorData.findOne().sort({ timestamp: -1 });
    if (latestData) {
      res.json({ pumpStatus: latestData.pumpStatus });
    } else {
      res.status(404).json({ message: 'Pump status not found' });
    }
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

app.post('/api/pump/control', async (req, res) => {
  try {
    const { status } = req.body;
    const sensorData = await SensorData.findOne().sort({ timestamp: -1 });
    if (sensorData) {
      sensorData.pumpStatus = status;
      await sensorData.save();
    }
    res.json({ success: true, pumpStatus: status });
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

app.get('/api/alerts', async (req, res) => {
  try {
    const latestData = await SensorData.findOne().sort({ timestamp: -1 });
    const alerts = [];

    if (latestData) {
      if (latestData.flameValue < 500) {
        alerts.push({ type: 'fire', message: 'Fire detected!' });
      }
      if (latestData.smokeValue > 300) {
        alerts.push({ type: 'smoke', message: 'High smoke levels detected!' });
      }
    }

    res.json(alerts);
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

app.listen(port, () => {
  console.log(`Server running on port ${port}`);
});
