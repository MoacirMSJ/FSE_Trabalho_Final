import React from 'react';
import CardComodo from './components/CardComodo/index.js';
import mqtt from 'mqtt';
function App() {
  const [messages, setMessages] = React.useState({});
  const [comodo, setComodo] = React.useState([]);


  const client = mqtt.connect('wss://test.mosquitto.org:8081');

  React.useEffect(() => {
    
    client.on('connect', function () { 
      client.subscribe('fse2020/170080366/dispositivos/#', 
          function (err) {
                   if (!err) {
                      console.log('Conectado')}
          })   
    })
  }, []);

  const trataCadastros =(topic, payload)=>{
    const m = JSON.parse(payload)
    if(m.cadastro){
      setComodo(prev=>[...prev, m.id])
      console.log(m.id)
    }

  }
  
  client.on('message',trataCadastros);

  return (
    <>
    <CardComodo comodo="Banheiro" />
    {/* <p>{comodo.map((message,index) => (
      <>
        <CardComodo comodo="Banheiro" />
      </>  
     ))}</p> */}
    
    
    </>
  );
}

export default App;
