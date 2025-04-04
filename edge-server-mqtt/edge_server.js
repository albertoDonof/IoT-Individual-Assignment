require('dotenv').config();
const mqtt = require('mqtt');
const fs = require('fs');

// MQTT Broker configuration with TLS
const brokerOptions = {
  host: '192.168.101.78', // Replace with your broker's URL
  port: 8883, // Typical secure MQTT port
  protocol: 'mqtts', // Use 'mqtts' for TLS
  rejectUnauthorized: false, // Verify the server certificate
  
  // If your broker requires client certificates:
  key: fs.readFileSync(process.env.CLIENT_KEY),
  cert: fs.readFileSync(process.env.CLIENT_CRT),
  ca: [fs.readFileSync(process.env.CA)],
  
};

// Topic to subscribe to
const topic = 'donofrio/individual/average';

const ping_topic = 'donofrio/individual/ping';
const response_topic = 'donofrio/individual/response';


// Create MQTT client
const client = mqtt.connect(brokerOptions);

// Connection event handlers
client.on('connect', () => {
  console.log(`Connected to MQTT broker at ${brokerOptions.host}`);

  // Subscribe to the topic
  client.subscribe(topic, (err) => {
    if (err) {
      console.error('Subscription error:', err);
    } else {
      console.log(`Subscribed to topic: ${topic}`);
    }
  });

  client.subscribe(ping_topic, (err) => {
    if (err) {
      console.error('Subscription error:', err);
    } else {
      console.log(`Subscribed to topic: ${ping_topic}`);
    }
  });
});

client.on('error', (err) => {
  console.error('Connection error:', err);
});

client.on('close', () => {
  console.log('Connection closed');
});

// Message event handler
client.on('message', (receivedTopic, message) => {
  // message needs convertion to string
  const value = message.toString();

  if (receivedTopic === ping_topic) {
    // This is a ping request - send pong with original timestamp
    const timestamp = value;
    client.publish(response_topic, timestamp);
    console.log(`Received ping, sending response (timestamp: ${timestamp})`);
  }
  else {
    console.log(`Received message on topic ${receivedTopic}:`, value);
  }

  
  
});

// Handle process termination
process.on('SIGINT', () => {
  client.end();
  process.exit();
});

console.log('MQTT TLS client started. Waiting for messages...');