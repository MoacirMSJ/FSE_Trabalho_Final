import React from 'react';
import mqtt from 'mqtt';
import { CSVLink, CSVDownload } from 'react-csv';
import moment from "moment";
import './styles.css'


const client = mqtt.connect('wss://test.mosquitto.org:8081');
function CardComodo(props) {
  console.log(props)

  const [messages, setMessages] = React.useState({});
  const [temperatura, setTemperatura] =  React.useState(0);
  const [umidade, setUmidade] =  React.useState(0);
  const [estado, setEstado] =  React.useState(0);
  const [lampada, setLampada] =  React.useState(0);
  const [alarme, setAlarme] =  React.useState(0);
  const [csvLog, setCsvLog] =  React.useState([]);

 const sub = ()=>{
  setCsvLog([["Data Hora","Comando","Requisicao"]])
  client.subscribe(`fse2020/170080366/${props.comodo}/#`, 
  function (err) {
            if (!err) {
              console.log(`fse2020/170080366/${props.comodo}`)
              console.log('Conectado')}
    }) 
    client.on('message',gerenciaMensagem);
 }


  React.useEffect(() => {
    sub()
  }, []);

  React.useEffect(()=>{


    client.publish(
      `fse2020/170080366/dispositivos/${props.id}`,
      `{ "tipo": 1, "lampada": ${lampada}}`
    )
    setCsvLog(prev=>[...prev,[`${moment().format("DD-MM-YYYY hh:mm:ss")}`,"lampada", `${lampada}`]])
  },[lampada])

  const gerenciaLampada= ()=>{
    if(lampada == 0){
      setLampada(1)
    }
    else{
      setLampada(0)
    }
    
  }

  React.useEffect(()=>{

    client.publish(
      `fse2020/170080366/dispositivos/${props.id}`,
      `{"tipo": 2, "alarme": ${alarme}}`
    )

    setCsvLog(prev=>[...prev,[`${moment().format("DD-MM-YYYY hh:mm:ss")}`,"alarme", `${alarme}`]])

  },[alarme])

  const gerenciaAlarme= ()=>{
    if(alarme == 0){
      setAlarme(1)
    }
    else{
      setAlarme(0)
    }
  }

  

  const gerenciaMensagem =(topic, payload)=>{
    const m = JSON.parse(payload);

    if(topic === `fse2020/170080366/${props.comodo}/temperatura` ){
      if(m.temperatura !== -1){
        setTemperatura(m.temperatura);
        
      }
      setCsvLog(prev=>[...prev,[`${moment().format("DD-MM-YYYY hh:mm:ss")}`,"temperatura", `${m.temperatura}`]])
      
      // console("Temp: ",temperatura)
    }else if(topic === `fse2020/170080366/${props.comodo}/umidade`){
      if(m.umidade !== -1){
        setUmidade(m.umidade);  
        
      }
      setCsvLog(prev=>[...prev,[`${moment().format("DD-MM-YYYY hh:mm:ss")}`,"umidade", `${m.umidade}`]])
    }
    else if(topic === `fse2020/170080366/${props.comodo}/estado`){
      setEstado(m.estado);
      setCsvLog(prev=>[...prev,[`${moment().format("DD-MM-YYYY hh:mm:ss")}`,"estado", "m.estado"]])
    }
    else if(topic === `fse2020/170080366/${props.comodo}/alarme`){
      // setEstado(m.estado);
      if(m.alarme === "acionado"){
        const audio = new Audio("http://www.healthfreedomusa.org/downloads/iMovie.app/Contents/Resources/iMovie%20%2708%20Sound%20Effects/Alarm.mp3");
        audio.play();
        setCsvLog(prev=>[...prev,[`${moment().format("DD-MM-YYYY hh:mm:ss")}`,"alarme", "acionado"]])
      }
      
    }
  }
  
  

  return (
    <div className ='container'>
      <h1 className = "title">
          {props.comodo}
       </h1>

      <h2>Temperatura: {temperatura}°C</h2>
      <h2>umidade: {umidade}%</h2>
      
      <div className="sub-container">
        <h2>lampada</h2>
      
        <input
            type="checkbox"
            defaultChecked={lampada}
            onChange={gerenciaLampada}
          />
      
      </div>
      <div className="sub-container">
        <h2>alarme</h2>
        <input
            type="checkbox"
            defaultChecked={alarme}
            onChange={gerenciaAlarme}
          />
      </div>

      <CSVLink  data={csvLog}>Baixar csv</CSVLink>;
    </div>
    
  );
}

export default CardComodo;
