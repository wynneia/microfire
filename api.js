const express = require('express');
const cors = require('cors');
const mongoose = require('mongoose');
const dotenv = require('dotenv');
const jwt = require('jsonwebtoken');
const bcrypt = require('bcryptjs');
const app = express();
const port = 3000;

dotenv.config();

app.use(cors());
app.use(express.json());

mongoose.connect(process.env.MONGO_URI)
  .then(() => console.log('Connected to MongoDB'))
  .catch((error) => console.error('Error connecting to MongoDB:', error));

const userSchema = new mongoose.Schema({
  username: { type: String, required: true, unique: true },
  password: { type: String, required: true }
});
const User = mongoose.model('User', userSchema);

const sensorDataSchema = new mongoose.Schema({
  flameValue: Number,
  smokeValue: Number,
  pumpStatus: Boolean,
  timestamp: { type: Date, default: Date.now }
});
const SensorData = mongoose.model('SensorData', sensorDataSchema);

const authenticateToken = (req, res, next) => {
  const authHeader = req.headers['authorization'];
  const token = authHeader && authHeader.split(' ')[1];

  if (token == null) {
    return res.status(401).json({ error: 'Token is required' });
  }

  jwt.verify(token, process.env.ACCESS_TOKEN_SECRET, (err, user) => {
    if (err) {
      return res.status(403).json({ error: 'Invalid token' });
    }
    req.user = user;
    next();
  });
};

app.post('/api/register', async (req, res) => {
  try {
    const { username, password } = req.body;

    const existingUser = await User.findOne({ username });
    if (existingUser) {
      return res.status(400).json({ error: 'User already exists' });
    }

    const hashedPassword = await bcrypt.hash(password, 10);

    const newUser = new User({
      username,
      password: hashedPassword
    });

    await newUser.save();
    res.status(201).json({ message: 'User registered successfully' });
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

app.post('/api/login', async (req, res) => {
  try {
    const { username, password } = req.body;

    const user = await User.findOne({ username });
    if (!user) {
      return res.status(401).json({ error: 'Invalid credentials' });
    }

    const validPassword = await bcrypt.compare(password, user.password);
    if (!validPassword) {
      return res.status(401).json({ error: 'Invalid credentials' });
    }

    const accessToken = jwt.sign({ username: user.username }, process.env.ACCESS_TOKEN_SECRET, { expiresIn: '1h' });
    res.json({ accessToken });
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

app.get('/api/sensor-data/latest', authenticateToken, async (req, res) => {
  try {
    const latestData = await SensorData.findOne().sort({ timestamp: -1 });
    res.json(latestData);
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

app.get('/api/sensor-data/history', authenticateToken, async (req, res) => {
  try {
    const history = await SensorData.find().sort({ timestamp: -1 }).limit(100);
    res.json(history);
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

app.post('/api/sensor-data', authenticateToken, async (req, res) => {
  try {
    const sensorData = new SensorData(req.body);
    await sensorData.save();
    res.status(201).json(sensorData);
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

app.get('/api/pump/status', authenticateToken, async (req, res) => {
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

app.post('/api/pump/control', authenticateToken, async (req, res) => {
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

app.get('/api/alerts', authenticateToken, async (req, res) => {
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
