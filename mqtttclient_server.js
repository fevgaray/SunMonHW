// The following code is the implementation of a MQTT client that subscribes to multiple MQTT topics
// additionally, once it receives each MQTT message it uploads the data to the according collection 
// in each database

const mqtt = require('mqtt');
const fs = require('fs');
const {MongoClient} = require('mongodb');

//MQTT Topic list: temperatures, camera, energy

// The following is the URI needed to connect to a MongoDB cloud database and add/see data to one of its collections
// for further explanation check  mongodb atlas connections
//mongodb+srv://<db_username>:<db_password>@sunmoncluster.oq80t.mongodb.net/?retryWrites=true&w=majority&appName=SunMonCluster
const uri = "<insert URI here>";
const client = mqtt.connect('mqtt://192.168.0.148', {clientId : 'nodejsMQTTclient'}); //Make sure to update IP to MQTT broker's IP as necessary
const atlasclient = new MongoClient(uri,{
    connectTimeoutMS: 15000 // 15 seconds
  });

// Function to show list of databases' names using the URI connection from mongo db cluster
async function listDatabases(client){
    databasesList = await client.db().admin().listDatabases();
 
    console.log("Databases:");
    databasesList.databases.forEach(db => console.log(` - ${db.name}`));
};


async function main() {
    

    client.on('connect', () =>{
        console.log('Connection established successfully to MQTT Broker!');
        // Subscribe to each topic of intereset from the MQTT broker (CHANGE AS DESIRED)
        client.subscribe('temperatures');
        client.subscribe('camera');
        client.subscribe('energy')
    });
    
    
    client.on('message', async (topic, message) => {
        console.log(`Received message on topic ${topic}`);
        try {
            if(topic == 'temperatures'){
                //console.log('Temperature from ESP received !')
                // Convert the message to a string, then parse it as JSON if it's in JSON format
                const messageString = message.toString();                
                try {
                    messageJson = JSON.parse(messageString); // Attempt to parse if the message is JSON
                    messageJson.timestamp = new Date().toLocaleString();
                    // CHANGE db(<name>) collection(<name>) AS DESIRED
                    const result = await atlasclient.db("pv_data").collection("weather").insertOne(messageJson);
                    console.log(`New temp data uploaded to db with Id: ${result.insertedId}`);
                } catch (err) {
                    // If message is not JSON, store it as a string
                    messageObject = { message: messageString };
                    console.log("Message was not a JSON and could not be uploaded to DB");
                }
                //const result = await atlasclient.db("weather_data").collection("Temperatures").insertOne(messageObject);
                //console.log(`New temp uploaded to db with Id: ${result.insertedId}`);
            }else if (topic == 'camera'){
                const messageString = message.toString();
                try{
                    console.log('Image from camera received !')
                    var messageJson = JSON.parse(message.toString());
                    //console.log('code from photo is ' + messageJson.photo);
                    messageJson.timestamp = new Date().toLocaleString();
                    // CHANGE db(<name>) collection(<name>) AS DESIRED
                    const result = await atlasclient.db("pv_data").collection("photos").insertOne(messageJson);
                    console.log(`New photo data uploaded to db with Id: ${result.insertedId}`);
                }catch (err) {
                    // If message is not JSON, store it as a string
                    messageObject = { message: messageString };
                    console.log("Message was not a JSON and could not be uploaded to DB");
                }
            }else if (topic == "energy"){
                //console.log('Temperature from ESP received !')
                //Convert the message to a string, then parse it as JSON if it's in JSON format
                const messageString = message.toString();
                try {
                    messageJson = JSON.parse(messageString); // Attempt to parse if the message is JSON
                    messageJson.timestamp = new Date().toLocaleString();
                    // CHANGE db(<name>) collection(<name>) AS DESIRED
                    const result = await atlasclient.db("pv_data").collection("energy").insertOne(messageJson);
                    console.log(`New current data uploaded to db with Id: ${result.insertedId}`);
                } catch (err) {
                    // If message is not JSON, store it as a string
                    messageObject = { message: messageString };
                    console.log("Message was not a JSON and could not be uploaded to DB");
                }
            }
        }
        catch(err){
            console.error('Error uploading message to DB:', err);
        }
      });
    
    client.on('error', (error)=>{
        console.log('ERROR')
        client.end()
    });


    try {
        // Connect to the MongoDB cluster
        await atlasclient.connect();
        // Make the appropriate DB calls
        await  listDatabases(atlasclient); 

    } catch (e) {
        console.error(e);
    } 

}

main().catch(console.error);
// When process is interrupted it closes its connection to the MongoDB cluster. (For example, a keyboard interruption ctrl + c)
process.on('SIGINT', async () => {
    console.log("Closing MongoDB connection...");
    await atlasclient.close();
    process.exit();
});
