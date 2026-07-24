
#pragma once

const char WEB_UI_HTML[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html lang="vi">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no">
<title>QUADCOPTER PNT</title>
<link rel="stylesheet" href="https://unpkg.com/leaflet@1.9.4/dist/leaflet.css" media="print" onload="this.media='all'">
<script defer src="https://unpkg.com/leaflet@1.9.4/dist/leaflet.js"></script>
<script defer src="https://cdn.plot.ly/plotly-2.32.0.min.js"></script>
<style>
:root{
  --bg:#07070d; --bg2:#0e0e18; --card:#12121e; --card2:#1a1a2e;
  --border:#252540; --text:#cdd6f4; --muted:#4a4a6a; --blue:#89b4fa;
  --cyan:#89dceb; --green:#a6e3a1; --yellow:#f9e2af; --red:#f38ba8;
  --orange:#fab387; --purple:#cba6f7; --pink:#f5c2e7; --teal:#94e2d5;
  --glow-cyan:0 0 16px #89dceb44; --glow-green:0 0 14px #a6e3a133;
  --glow-red:0 0 14px #f38ba833; --glow-blue:0 0 14px #89b4fa33;
  --radius:14px; --radius-sm:9px; --radius-xs:6px;
}
*{box-sizing:border-box;margin:0;padding:0}
body{background:var(--bg);color:var(--text);font-family:'Segoe UI',system-ui,sans-serif;font-size:14px;min-height:100vh;padding-bottom:50px;}
header{background:linear-gradient(135deg,#1a1a30 0%,#0e0e1a 100%);border-bottom:1px solid var(--border);padding:10px 16px 10px 8px;display:flex;flex-direction:column;gap:8px;position:sticky;top:0;z-index:100;backdrop-filter:blur(20px);box-shadow:0 2px 20px #00000060;}
.hdr-top{display:flex;align-items:center;justify-content:space-between;gap:14px}
.hdr-right{display:flex;align-items:center;justify-content:flex-end;gap:8px;flex-wrap:wrap;min-width:0}
.hdr-status,.hdr-temps{display:flex;align-items:center;justify-content:flex-end;gap:6px;flex-wrap:wrap}
.hdr-temp{display:inline-flex;align-items:baseline;gap:6px;padding:4px 9px;border-radius:999px;background:#1a1a2e;border:1px solid var(--border);font-size:10px;font-weight:800;letter-spacing:.8px;color:#fff;white-space:nowrap}
.hdr-temp b{font-size:12px;color:#fff;font-variant-numeric:tabular-nums}
.hdr-batt{display:inline-flex;align-items:center;gap:6px;padding:6px 10px;border-radius:999px;background:#a6e3a112;border:1px solid #a6e3a130;color:var(--green);font-size:11px;font-weight:900;white-space:nowrap;font-variant-numeric:tabular-nums}
.hdr-batt.warn{background:#f9e2af14;border-color:#f9e2af40;color:var(--yellow)}
.hdr-batt.fail{background:#f38ba814;border-color:#f38ba840;color:var(--red)}
.signal-chip{display:inline-flex;align-items:center;gap:7px;padding:5px 9px;border-radius:999px;background:#1a1a2e;border:1px solid var(--border);font-size:10px;font-weight:900;letter-spacing:.6px;color:#fff;white-space:nowrap;transition:all .25s;}
.signal-chip.ok{border-color:#a6e3a136;background:#a6e3a10f;color:var(--green)}
.signal-chip.warn{border-color:#f9e2af42;background:#f9e2af12;color:var(--yellow)}
.signal-chip.fail{border-color:#f38ba832;background:#f38ba80f;color:var(--red)}
.signal-bars{display:inline-flex;align-items:flex-end;gap:2px;height:13px}
.signal-bars i{display:block;width:4px;border-radius:3px;background:#3a3a58;opacity:.9}
.signal-bars i:nth-child(1){height:4px}.signal-bars i:nth-child(2){height:7px}.signal-bars i:nth-child(3){height:10px}.signal-bars i:nth-child(4){height:13px}
.signal-chip.ok .signal-bars i.on{background:var(--green);box-shadow:0 0 8px rgba(166,227,161,.55)}
.signal-chip.warn .signal-bars i.on{background:var(--yellow);box-shadow:0 0 8px rgba(249,226,175,.45)}
.signal-chip.fail .signal-bars i.on{background:var(--red);box-shadow:0 0 8px rgba(243,139,168,.45)}
.signal-chip b{font-size:10px;font-variant-numeric:tabular-nums;color:#fff}
.logo{display:flex;align-items:center;gap:8px;min-height:38px;flex-shrink:0}
.logo-icon{font-size:26px;animation:hue 6s linear infinite}
@keyframes hue{0%{filter:hue-rotate(0deg) drop-shadow(0 0 10px #89dceb)}100%{filter:hue-rotate(360deg) drop-shadow(0 0 10px #89dceb)}}
.logo-text{display:flex;flex-direction:column;justify-content:center;line-height:1.05;text-align:left;font-weight:900;background:linear-gradient(90deg,var(--cyan),var(--blue),var(--purple));-webkit-background-clip:text;-webkit-text-fill-color:transparent;background-clip:text;}
.logo-main{font-size:16px;letter-spacing:.8px}
.logo-sub{font-size:12px;letter-spacing:2px;margin-top:2px}
#hdr-arm{padding:6px 14px;border-radius:20px;font-size:11px;font-weight:800;letter-spacing:1.5px;border:1px solid;transition:.3s;white-space:nowrap;flex-shrink:0;}
@media(max-width:640px){.hdr-top{align-items:flex-start;flex-direction:column}.hdr-status,.hdr-temps,.hdr-right{justify-content:flex-start}}
.main{max-width:920px;margin:0 auto;padding:12px;display:grid;grid-template-columns:1fr 1fr;gap:11px;}
@media(max-width:640px){.main{grid-template-columns:1fr}}
.full{grid-column:1/-1}
.card{background:linear-gradient(180deg,#151528,#10101d);border:1px solid #303052;border-radius:var(--radius);padding:14px;transition:border-color .3s,box-shadow .3s;}
.card:hover{border-color:#46466f;box-shadow:0 4px 24px #00000040,0 0 18px #89dceb12}
.card-title{font-size:11px;font-weight:900;letter-spacing:2.4px;color:var(--cyan);text-transform:uppercase;margin-bottom:14px;display:flex;align-items:center;gap:9px;text-shadow:0 0 12px rgba(137,220,235,.28);}
.card-title::before{content:'';display:block;width:4px;height:18px;border-radius:3px;background:linear-gradient(180deg,var(--cyan),var(--blue));box-shadow:0 0 10px rgba(137,220,235,.5);flex-shrink:0;}
.badge{display:inline-flex;align-items:center;gap:5px;padding:4px 9px;border-radius:20px;font-size:11px;font-weight:700;border:1px solid transparent;white-space:nowrap;flex-shrink:0;line-height:1.3;transition:all .3s;}
.badge .dot{width:6px;height:6px;border-radius:50%;box-shadow:0 0 6px currentColor;flex-shrink:0;}
.badge-ok{background:#a6e3a112;color:var(--green);border-color:#a6e3a125}
.badge-ok .dot{background:var(--green);color:var(--green)}
.badge-fail{background:#f38ba810;color:var(--red);border-color:#f38ba820}
.badge-fail .dot{background:var(--red);color:var(--red)}
.badge-armed{background:#fab38718;color:var(--orange);border-color:#fab38740;box-shadow:0 0 12px #fab38730,var(--glow-red)}
.badge-disarmed{background:#1a1a2e;color:var(--muted);border-color:var(--border)}
.sensor-grid{display:grid;grid-template-columns:repeat(3,1fr);gap:8px}
.sensor-box{background:var(--card2);border:1px solid var(--border);border-radius:var(--radius-sm);padding:11px 8px;text-align:center;transition:border-color .2s;position:relative;overflow:hidden;}
.sensor-box::before{content:'';position:absolute;top:0;left:0;right:0;height:2px;background:linear-gradient(90deg,transparent,currentColor,transparent);opacity:.3;}
.sensor-label{font-size:9px;color:#fff;letter-spacing:1.5px;text-transform:uppercase}
.sensor-val{font-size:22px;font-weight:800;margin:4px 0 2px;font-variant-numeric:tabular-nums;line-height:1}
.sensor-unit{font-size:9px;color:#fff}
.chart-wrap{background:#0d0e1a;border:1px solid #303052;border-radius:var(--radius-sm);padding:7px;margin-bottom:10px;}
.chart-header{display:flex;justify-content:space-between;align-items:center;margin-bottom:4px;}
.chart-label{font-size:10px;color:#fff;text-transform:uppercase;letter-spacing:1.4px;font-weight:800}
.chart-legend{display:flex;gap:8px;font-size:9px}

.attitude{position:relative;width:100%;height:280px;background:linear-gradient(180deg,#10162a 0%,#0a0c17 48%,#05060b 100%);border:1px solid var(--border);border-radius:var(--radius-sm);overflow:hidden;margin-bottom:10px;display:flex;align-items:center;justify-content:center;perspective:980px;isolation:isolate;}
.attitude::before{content:'';position:absolute;left:-22%;right:-22%;bottom:-48px;height:64%;background:linear-gradient(rgba(137,220,235,.16) 1px,transparent 1px),linear-gradient(90deg,rgba(137,220,235,.16) 1px,transparent 1px);background-size:32px 32px;transform:rotateX(64deg);transform-origin:50% 100%;opacity:.34;}
.attitude::after{content:'';position:absolute;left:10%;right:10%;top:50%;height:1px;background:linear-gradient(90deg,transparent,#f9e2af90,transparent);box-shadow:0 -60px 0 #89dceb1f,0 60px 0 #89dceb14;opacity:.9;}
.hud-ring{position:absolute;width:205px;height:205px;border:1px solid rgba(137,220,235,.28);border-radius:50%;transform:rotateX(64deg);box-shadow:inset 0 0 20px rgba(137,220,235,.08),0 0 30px rgba(137,220,235,.09);}
.hud-ring::before,.hud-ring::after{content:'';position:absolute;background:rgba(137,220,235,.23);left:50%;top:50%;transform:translate(-50%,-50%);}
.hud-ring::before{width:1px;height:100%}
.hud-ring::after{width:100%;height:1px}
.drone-scene{position:relative;z-index:2;width:220px;height:220px;transform-style:preserve-3d;transform:rotateX(62deg) translateY(18px);will-change:transform;}
.drone-shadow{position:absolute;left:50%;top:50%;width:178px;height:84px;border-radius:50%;background:radial-gradient(ellipse,rgba(0,0,0,.55),rgba(0,0,0,.24) 42%,rgba(0,0,0,0) 72%);transform:translate(-50%,-50%) translateZ(-36px);filter:blur(5px);opacity:.42;will-change:transform,opacity;}
.drone-3d{position:absolute;left:50%;top:50%;width:1px;height:1px;transform-style:preserve-3d;will-change:transform;filter:drop-shadow(0 18px 16px rgba(0,0,0,.50));}
.d3-body{position:absolute;width:78px;height:48px;background:linear-gradient(145deg,#2b3154,#101522 58%,#080b14);border:2px solid rgba(137,220,235,.92);top:-24px;left:-39px;border-radius:14px 14px 18px 18px;display:flex;align-items:center;justify-content:center;color:var(--cyan);font-weight:900;font-size:11px;letter-spacing:1.5px;box-shadow:0 0 18px rgba(137,220,235,.35),inset 0 0 16px rgba(137,220,235,.10);transform:translateZ(32px);}
.d3-body::before{content:'';position:absolute;inset:6px 16px auto;height:5px;border-radius:5px;background:linear-gradient(90deg,var(--red),var(--yellow),var(--cyan));opacity:.9;}
.d3-body::after{content:'';position:absolute;bottom:-8px;left:18px;right:18px;height:10px;border-radius:0 0 12px 12px;background:linear-gradient(180deg,#161b31,#070912);border:1px solid rgba(137,220,235,.34);border-top:0;}
.d3-arm{position:absolute;width:12px;height:168px;background:linear-gradient(90deg,#20263f,#77819e,#20263f);top:-84px;left:-6px;transform-origin:center;border-radius:999px;box-shadow:0 0 11px rgba(137,220,235,.18);transform-style:preserve-3d;}
.d3-arm::after{content:'';position:absolute;left:3px;right:3px;top:8px;bottom:8px;border-radius:999px;background:linear-gradient(180deg,rgba(255,255,255,.20),transparent 24%,rgba(0,0,0,.20));opacity:.65;}
.d3-arm-fr{transform:rotate(45deg);} .d3-arm-fl{transform:rotate(-45deg);} .d3-arm-br{transform:rotate(135deg);} .d3-arm-bl{transform:rotate(-135deg);}
.d3-nose{position:absolute;width:0;height:0;border-left:14px solid transparent;border-right:14px solid transparent;border-bottom:34px solid rgba(243,139,168,.95);top:-66px;left:-14px;filter:drop-shadow(0 0 12px rgba(243,139,168,.6));transform:translateZ(42px);}
.d3-motor{position:absolute;width:54px;height:54px;top:-27px;left:-27px;border-radius:50%;background:radial-gradient(circle at 35% 30%,#3c4466,#111726 56%,#05070d);border:1px solid rgba(137,220,235,.55);box-shadow:0 0 var(--motor-glow,12px) var(--motor-color,rgba(137,220,235,.20));transform-style:preserve-3d;will-change:box-shadow;}
.d3-motor::after{content:'';position:absolute;inset:17px;border-radius:50%;background:#05060b;border:1px solid rgba(205,214,244,.32);box-shadow:inset 0 0 8px #000;}
.d3-motor-front{border-color:rgba(243,139,168,.75);--motor-color:rgba(243,139,168,.24);}
.d3-motor-fr{transform:translate(72px,-72px) translateZ(22px);}
.d3-motor-fl{transform:translate(-72px,-72px) translateZ(22px);}
.d3-motor-br{transform:translate(72px,72px) translateZ(22px);}
.d3-motor-bl{transform:translate(-72px,72px) translateZ(22px);}
.d3-prop-disc{position:absolute;inset:-16px;border-radius:50%;background:transparent;border:0;opacity:1;transform:translateZ(12px);}
.d3-prop-disc::before{content:'';position:absolute;left:50%;top:50%;width:88px;height:11px;border-radius:999px;background:linear-gradient(90deg,rgba(137,220,235,.06),rgba(137,220,235,.72),rgba(205,214,244,.88),rgba(137,220,235,.72),rgba(137,220,235,.06));transform:translate(-50%,-50%);box-shadow:0 0 10px rgba(137,220,235,.18);}
.d3-prop-disc::after{content:'';position:absolute;left:50%;top:50%;width:58px;height:3px;border-radius:999px;background:rgba(255,255,255,.42);transform:translate(-50%,-50%);}
.d3-motor-front .d3-prop-disc::before{background:linear-gradient(90deg,rgba(243,139,168,.06),rgba(243,139,168,.72),rgba(245,194,231,.88),rgba(243,139,168,.72),rgba(243,139,168,.06));box-shadow:0 0 10px rgba(243,139,168,.22);}
.d3-prop-disc.spinning{animation-name:spinPropCw;animation-duration:var(--spin-dur,.5s);animation-timing-function:linear;animation-iteration-count:infinite;background:conic-gradient(from 0deg,rgba(137,220,235,0),rgba(137,220,235,.62),rgba(137,220,235,0) 20%,rgba(137,220,235,0) 50%,rgba(137,220,235,.62),rgba(137,220,235,0) 70%);border:2px solid rgba(137,220,235,.20);opacity:var(--prop-opacity,.72);filter:blur(var(--prop-blur,0px));}
.d3-prop-disc.spinning.prop-ccw{animation-name:spinPropCcw;}
.d3-motor-front .d3-prop-disc.spinning{background:conic-gradient(from 0deg,rgba(243,139,168,0),rgba(243,139,168,.68),rgba(243,139,168,0) 20%,rgba(243,139,168,0) 50%,rgba(243,139,168,.68),rgba(243,139,168,0) 70%);border-color:rgba(243,139,168,.24);}
.d3-prop-disc.spinning::before,.d3-prop-disc.spinning::after{opacity:0}
.d3-motor-label{position:absolute;left:50%;top:-24px;transform:translateX(-50%) translateZ(28px);font-size:9px;font-weight:900;letter-spacing:.8px;color:var(--cyan);text-shadow:0 0 8px rgba(137,220,235,.7);pointer-events:none;z-index:3;}
.d3-motor-front .d3-motor-label{color:var(--pink);text-shadow:0 0 8px rgba(243,139,168,.75);}
@keyframes spinPropCw{from{transform:translateZ(9px) rotate(0deg)}to{transform:translateZ(9px) rotate(360deg)}}
@keyframes spinPropCcw{from{transform:translateZ(9px) rotate(0deg)}to{transform:translateZ(9px) rotate(-360deg)}}
.ch-rows{display:flex;flex-direction:column;gap:6px}
.ch-row{display:grid;grid-template-columns:54px 1fr 46px;align-items:center;gap:6px}
.ch-name{font-size:10px;color:#8f96c4;font-weight:800;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}
.ch-val{font-size:11px;color:var(--cyan);text-align:right;font-weight:800;font-variant-numeric:tabular-nums}
.bar-track{height:8px;background:#0a0a14;border-radius:4px;overflow:hidden;border:1px solid var(--border);min-width:0}
.bar-fill{height:100%;border-radius:4px;background:linear-gradient(90deg,var(--cyan),var(--blue));transition:width .08s ease-out}
.drone-wrap{display:flex;justify-content:center;margin-bottom:12px}
.drone-diagram{position:relative;width:190px;height:150px}
.motor-node{position:absolute;width:48px;height:48px;border-radius:50%;background:var(--card2);border:2px solid var(--border);display:flex;flex-direction:column;align-items:center;justify-content:center;transition:border-color .2s,box-shadow .2s;}
.motor-node .m-label{color:var(--muted);font-weight:800;font-size:8px}
.motor-node .m-val{color:var(--cyan);font-weight:800;font-size:12px;font-variant-numeric:tabular-nums}
.motor-node .m-dir{font-size:10px;margin-left:2px;color:var(--pink);vertical-align:-1px}
.motor-node.tl{top:2px;left:2px}.motor-node.tr{top:2px;right:2px}.motor-node.bl{bottom:2px;left:2px}.motor-node.br{bottom:2px;right:2px}
.center-dot{position:absolute;top:50%;left:50%;transform:translate(-50%,-50%);width:24px;height:24px;border-radius:50%;background:var(--card2);border:2px solid var(--cyan);box-shadow:var(--glow-cyan);display:flex;align-items:center;justify-content:center;font-size:10px;color:var(--cyan);font-weight:800;}
.btn-grid{display:grid;grid-template-columns:1fr 1fr;gap:10px;margin-bottom:10px}
.btn{padding:12px 8px;border-radius:var(--radius-sm);border:1px solid;cursor:pointer;font-size:12px;font-weight:800;letter-spacing:.5px;transition:all .2s;background:transparent;outline:none;}
.btn-arm{color:var(--green);border-color:#a6e3a140;background:linear-gradient(135deg,#a6e3a108,#a6e3a118)}
.btn-arm:hover{box-shadow:0 0 16px #a6e3a130;border-color:#a6e3a180}
.btn-disarm{color:var(--red);border-color:#f38ba840;background:linear-gradient(135deg,#f38ba808,#f38ba818)}
.btn-disarm:hover{box-shadow:var(--glow-red);border-color:#f38ba880}
.btn-action{color:var(--yellow);border-color:#f9e2af25;background:#f9e2af06;font-size:11px}
.btn-action:hover{border-color:#f9e2af50;box-shadow:0 0 10px #f9e2af20}
.pid-tune-grid{display:grid;grid-template-columns:repeat(3,minmax(0,1fr));gap:12px}
@media(max-width:900px){.pid-tune-grid{grid-template-columns:repeat(2,minmax(0,1fr))}}
@media(max-width:640px){.pid-tune-grid{grid-template-columns:1fr}}
.pid-tune-card{--axis:#89dceb;background:linear-gradient(180deg,#17182b,#10111f);border:1px solid #303052;border-top:3px solid var(--axis);border-radius:8px;padding:12px;min-width:0;box-shadow:0 8px 22px rgba(0,0,0,.22)}
.pid-tune-head{display:flex;align-items:center;justify-content:space-between;gap:8px;margin-bottom:10px}
.pid-tune-name{font-size:13px;font-weight:900;color:var(--axis)}
.pid-tune-id{font-size:10px;color:#8f96c4;background:#24263a;border-radius:5px;padding:3px 6px;font-variant-numeric:tabular-nums}
.pid-live-grid{display:grid;grid-template-columns:repeat(3,minmax(0,1fr));gap:6px;margin-bottom:10px}
.pid-live-box{background:#202235;border:1px solid #252840;border-radius:6px;padding:7px 8px;min-width:0}
.pid-live-label{display:block;font-size:9px;color:#8f96c4;font-weight:900;letter-spacing:.8px;text-transform:uppercase;margin-bottom:5px;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}
.pid-live-val{display:block;font-size:15px;color:#fff;font-weight:900;font-variant-numeric:tabular-nums;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}
.pid-live-val.err{color:#ff7f92}
.pid-tune-row{display:grid;grid-template-columns:24px minmax(0,1fr);align-items:center;gap:8px;margin:8px 0}
.pid-tune-row label{font-size:11px;color:#aab4e5;font-weight:900}
.pid-num{width:100%;background:#24263a;border:1px solid #2a2d47;border-radius:6px;color:#fff;padding:6px 7px;font-size:13px;font-weight:800;outline:none;font-variant-numeric:tabular-nums}
.pid-num:focus{border-color:var(--axis);box-shadow:0 0 0 2px color-mix(in srgb,var(--axis) 20%,transparent)}
.pid-update{width:100%;margin-top:8px;padding:9px 8px;border-radius:6px;border:1px solid var(--axis);background:transparent;color:var(--axis);font-weight:900;cursor:pointer}
.pid-mini-chart{width:100%;height:86px;display:block;margin-top:10px;background:#121424;border-top:1px solid #262943}
.off-row{display:grid;grid-template-columns:60px 1fr auto;align-items:center;gap:8px;padding:10px 0;border-bottom:1px solid #12121e}
.off-label{font-size:12px;color:#dce5ff;font-weight:800}
.off-input{width:82px;background:var(--card2);border:1px solid var(--border);border-radius:var(--radius-xs);color:var(--text);padding:7px 8px;font-size:13px;outline:none;transition:border-color .2s;text-align:center;}
.off-input:focus{border-color:var(--cyan)}
.off-btns{display:flex;gap:4px}
.off-btn{width:32px;height:32px;border-radius:var(--radius-xs);border:1px solid var(--border);background:var(--card2);color:var(--cyan);font-size:16px;font-weight:800;cursor:pointer;display:flex;align-items:center;justify-content:center;transition:all .15s;line-height:1;}
.off-btn:hover{border-color:var(--cyan);box-shadow:var(--glow-cyan)}
#toast{position:fixed;bottom:20px;right:16px;padding:10px 18px;border-radius:var(--radius-sm);font-size:13px;font-weight:700;background:var(--card2);border:1px solid;opacity:0;transition:opacity .25s,transform .25s;transform:translateY(10px);z-index:999;}
#toast.show{opacity:1;transform:translateY(0)}
#toast.ok{color:var(--green);border-color:#a6e3a130}
#toast.err{color:var(--red);border-color:#f38ba830}
.label-row{display:flex;justify-content:space-between;align-items:baseline;margin-bottom:7px}
.label-name{font-size:12px;color:#fff;font-weight:800}
.label-val{font-size:15px;font-weight:900;font-variant-numeric:tabular-nums}
.prog-track{height:8px;background:#0a0a14;border-radius:4px;overflow:hidden;border:1px solid var(--border)}
.prog-fill{height:100%;border-radius:4px;transition:width .25s ease}
.data-grid{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:8px}
.data-box{background:var(--card2);border:1px solid var(--border);border-radius:var(--radius-sm);padding:9px 8px;min-width:0}
.data-label{font-size:9px;color:#fff;letter-spacing:1.2px;text-transform:uppercase;font-weight:800;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}
.data-value{font-size:15px;font-weight:900;margin-top:4px;font-variant-numeric:tabular-nums;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}
.compass-panel{display:grid;grid-template-columns:180px 1fr;gap:12px;align-items:center}
@media(max-width:640px){.compass-panel{grid-template-columns:1fr}.compass-dial{margin:0 auto}}
.compass-dial{position:relative;width:180px;height:180px;border-radius:50%;background:radial-gradient(circle at 50% 50%,#1b2238 0%,#0c0f1c 58%,#05070d 100%);border:1px solid #303052;box-shadow:inset 0 0 24px rgba(137,220,235,.12),0 0 18px rgba(137,220,235,.12)}
.compass-dial::before{content:'';position:absolute;inset:16px;border:1px solid rgba(137,220,235,.20);border-radius:50%}
.compass-dial::after{content:'';position:absolute;left:50%;top:50%;width:9px;height:9px;border-radius:50%;background:var(--cyan);transform:translate(-50%,-50%);box-shadow:0 0 12px rgba(137,220,235,.75)}
.compass-mark{position:absolute;font-size:12px;font-weight:900;color:#fff;text-shadow:0 0 8px rgba(137,220,235,.5)}
.compass-mark.n{left:50%;top:8px;transform:translateX(-50%);color:var(--red)}
.compass-mark.e{right:12px;top:50%;transform:translateY(-50%)}
.compass-mark.s{left:50%;bottom:8px;transform:translateX(-50%)}
.compass-mark.w{left:12px;top:50%;transform:translateY(-50%)}
.compass-needle{position:absolute;left:50%;top:50%;width:5px;height:70px;border-radius:5px 5px 2px 2px;background:linear-gradient(180deg,var(--red),var(--yellow));transform-origin:50% 100%;transform:translate(-50%,-100%) rotate(0deg);box-shadow:0 0 13px rgba(243,139,168,.45);transition:transform .18s ease-out}
.compass-needle::after{content:'';position:absolute;left:50%;bottom:-54px;width:3px;height:54px;border-radius:2px;background:linear-gradient(180deg,var(--cyan),rgba(137,220,235,.25));transform:translateX(-50%)}
.heading-readout{text-align:center;margin-top:8px;font-size:24px;font-weight:900;font-variant-numeric:tabular-nums;color:var(--cyan)}
.heading-readout span{display:block;font-size:11px;letter-spacing:2px;color:#fff;margin-top:2px}
.map-wrap{background:#0d0e1a;border:1px solid #303052;border-radius:var(--radius-sm);padding:8px;margin-top:10px}
.gps-map-shell{position:relative;width:100%;height:380px;border-radius:8px;overflow:hidden;background:#26324a}
#gps-map,#gps-offline-map{position:absolute;inset:0;width:100%;height:100%;display:block}
#gps-map{z-index:1}
#gps-offline-map{z-index:0;display:none}
.gps-map-shell.offline-on #gps-map{display:none}
.gps-map-shell.offline-on #gps-offline-map{display:block;z-index:1}
.gps-map-overlay{position:absolute;left:10px;top:10px;z-index:410;font-size:11px;font-weight:900;color:#dce5ff;background:rgba(7,9,17,.62);border:1px solid #26324a;border-radius:6px;padding:6px 8px;backdrop-filter:blur(8px)}
.gps-map-offline{position:absolute;top:50%;left:50%;transform:translate(-50%,-50%);color:#93a4c7;text-align:center;display:none;z-index:411;font-size:14px;background:rgba(7,9,17,.72);border:1px solid #26324a;border-radius:8px;padding:10px 14px}
.gps-map-shell.offline-on .gps-map-offline{display:block}
.leaflet-container{font-family:Arial,sans-serif;background:#26324a}
.custom-pin{background:none;border:none}
.gps-drone-pin{position:relative;width:28px;height:28px}
.gps-pin-dot{position:absolute;left:50%;top:50%;width:16px;height:16px;background:#12b886;border:3px solid #fff;border-radius:50%;box-shadow:0 0 10px rgba(18,184,134,.8);transform:translate(-50%,-50%)}
.gps-pin-arrow{position:absolute;left:50%;top:50%;width:0;height:0;border-left:6px solid transparent;border-right:6px solid transparent;border-bottom:18px solid #ff6b8a;transform-origin:50% 80%;filter:drop-shadow(0 0 6px rgba(255,107,138,.65));margin-left:-6px;margin-top:-20px}
.map-toolbar{display:flex;align-items:center;justify-content:space-between;gap:10px;margin-top:8px;flex-wrap:wrap}
.map-stat{font-size:11px;color:#fff;font-weight:800;font-variant-numeric:tabular-nums}
.flow-map-shell{position:relative;width:100%;height:330px;border-radius:8px;overflow:hidden;background:#060914;border:1px solid #26324a}
#flow-map{width:100%;height:100%;display:block}
.flow-map-overlay{position:absolute;left:10px;top:10px;z-index:2;font-size:11px;font-weight:900;color:#dce5ff;background:rgba(7,9,17,.66);border:1px solid #26324a;border-radius:6px;padding:6px 8px;backdrop-filter:blur(8px)}
.flow-map-actions{position:absolute;right:10px;bottom:10px;z-index:2}
hr.divider{border:none;border-top:1px solid var(--border);margin:12px 0}

</style>
</head>
<body>

<header>
  <div class="hdr-top">
    <div class="logo">
      <span class="logo-icon">&#x1F681;</span>
      <div class="logo-text"><span class="logo-main">QUADCOPTER PNT</span><span class="logo-sub">42300214</span></div>
    </div>
    <div class="hdr-right" style="display:flex;align-items:center;gap:10px">
      <div class="hdr-batt" id="hdr-batt">&#x1F50B; --.-V &middot; --%</div>
      <div id="hdr-arm" class="badge badge-disarmed">DISARMED</div>
    </div>
  </div>
  <div class="hdr-status">
    <span class="badge badge-fail" id="b-pid"><span class="dot"></span><span>PID OFF</span></span>
    <span class="badge badge-fail" id="b-icm"><span class="dot"></span><span>MPU6500</span></span>
    <span class="badge badge-fail" id="b-alt"><span class="dot"></span><span>ALT SENSOR</span></span>
    <span class="badge badge-fail" id="b-gps"><span class="dot"></span><span>GPS</span></span>
    <span class="badge badge-fail" id="b-compass"><span class="dot"></span><span>COMPASS</span></span>
    <span class="badge badge-fail" id="b-sbus"><span class="dot"></span><span>SBUS</span></span>
    <span class="signal-chip fail" id="sig-wifi"><span>WiFi ESP</span><span class="signal-bars" id="bars-wifi"><i></i><i></i><i></i><i></i></span><b id="sig-wifi-val">--%</b></span>
    <span class="signal-chip fail" id="sig-sbus"><span>Tay cầm</span><span class="signal-bars" id="bars-sbus"><i></i><i></i><i></i><i></i></span><b id="sig-sbus-val">--%</b></span>
    <span class="badge badge-fail" id="b-batt-warn" style="display:none"><span class="dot"></span><span id="t-batt-warn">PIN YẾU</span></span>
    <span class="badge badge-armed" id="b-autoland" style="display:none"><span class="dot"></span><span>HẠ CÁNH</span></span>
    <span class="badge badge-fail" id="b-fs" style="display:none"><span class="dot"></span><span>&#x26A0; FAILSAFE</span></span>
  </div>
  <div class="hdr-temps">
    <span class="hdr-temp">MPU <b id="temp-mpu">--&deg;C</b></span>
    <span class="hdr-temp">ESP <b id="temp-esp">--&deg;C</b></span>
  </div>
</header>

<div class="main">

  <div class="card full" style="padding:0;overflow:hidden;background:#0f1422">
    <div style="display:flex;justify-content:space-between;padding:10px 12px;border-bottom:1px solid #26324a">
      <b style="color:#93a4c7;font-size:14px;letter-spacing:1px;text-transform:uppercase">BAN DO XY CAM BIEN</b>
      <span id="flow-map-badge" class="badge badge-fail">FLOW OFF</span>
    </div>
    <div style="padding:12px">
      <div class="flow-map-shell" style="height:380px">
        <div id="flow-map" style="width:100%;height:100%"></div>
        <div class="flow-map-actions" style="z-index:999; display:flex; gap:6px; align-items:center; background:rgba(7,9,17,.7); padding:6px; border-radius:6px; border:1px solid #26324a">
          <button class="btn btn-action" style="padding:4px 8px; background:rgba(0,0,0,.62); border:1px solid var(--border); color:#fff; border-radius:4px" onclick="resetFlowMapWeb()">Reset XY</button>
        </div>
      </div>
      <div class="data-grid" style="margin-top:10px">
        <div class="data-box"><div class="data-label">X</div><div class="data-value" id="flow-pos-x">0.0</div></div>
        <div class="data-box"><div class="data-label">Y</div><div class="data-value" id="flow-pos-y">0.0</div></div>
        <div class="data-box"><div class="data-label">Vx/Vy</div><div class="data-value" id="flow-vel-xy">0.0/0.0</div></div>
        <div class="data-box"><div class="data-label">Q/Age</div><div class="data-value" id="flow-q-age">--</div></div>
      </div>
    </div>
  </div>

  <div class="card">
    <div class="card-title">Cảm biến IMU</div>
    <div class="attitude" id="att-vis">
      <div class="hud-ring"></div>
      <div class="drone-scene">
        <div class="drone-shadow" id="drone-shadow"></div>
        <div class="drone-3d" id="drone-3d">
          <div class="d3-arm d3-arm-fr"></div>
          <div class="d3-arm d3-arm-fl"></div>
          <div class="d3-arm d3-arm-br"></div>
          <div class="d3-arm d3-arm-bl"></div>
          <div class="d3-motor d3-motor-front d3-motor-fl" id="d3-motor3"><div class="d3-prop-disc prop-cw" id="d3-prop3"></div><span class="d3-motor-label">M3</span></div>
          <div class="d3-motor d3-motor-front d3-motor-fr" id="d3-motor2"><div class="d3-prop-disc prop-ccw" id="d3-prop2"></div><span class="d3-motor-label">M2</span></div>
          <div class="d3-motor d3-motor-bl" id="d3-motor4"><div class="d3-prop-disc prop-ccw" id="d3-prop4"></div><span class="d3-motor-label">M4</span></div>
          <div class="d3-motor d3-motor-br" id="d3-motor1"><div class="d3-prop-disc prop-cw" id="d3-prop1"></div><span class="d3-motor-label">M1</span></div>
          <div class="d3-nose"></div>
          <div class="d3-body">FC</div>
        </div>
      </div>
    </div>
    <div class="chart-wrap">
      <div class="chart-header">
        <span class="chart-label">Góc nghiêng (&deg;)</span>
        <div class="chart-legend">
          <span style="color:var(--red)">&#x25CF; Roll</span>
          <span style="color:var(--green)">&#x25CF; Pitch</span>
        </div>
      </div>
      <canvas id="waveChart" width="400" height="120" style="width:100%;height:120px;display:block;"></canvas>
    </div>
    <div class="sensor-grid">
      <div class="sensor-box" style="color:var(--red)"><div class="sensor-label">Roll</div><div class="sensor-val" id="sv-roll">0.0</div><div class="sensor-unit">&deg;</div></div>
      <div class="sensor-box" style="color:var(--green)"><div class="sensor-label">Pitch</div><div class="sensor-val" id="sv-pitch">0.0</div><div class="sensor-unit">&deg;</div></div>
      <div class="sensor-box" style="color:var(--purple)"><div class="sensor-label">Yaw</div><div class="sensor-val" id="sv-yaw">0.0</div><div class="sensor-unit">&deg;</div></div>
    </div>
  </div>

  <div class="card">
    <div class="card-title">Pin &amp; ALT SENSOR</div>
    <div style="margin-bottom:14px">
      <div class="label-row"><span class="label-name">LiPo 3S</span><span class="label-val" id="sv-batt">--.-V</span></div>
      <div class="prog-track"><div class="prog-fill" id="bar-batt" style="width:0;background:linear-gradient(90deg,#f38ba8,#f9e2af,#a6e3a1)"></div></div>
      <div style="display:flex;justify-content:space-between;margin-top:4px">
        <span style="font-size:10px;color:#fff">10.5V</span>
        <span id="sv-battPct" style="font-size:13px;font-weight:800;color:var(--green)">--%</span>
        <span style="font-size:10px;color:#fff">12.6V</span>
      </div>
    </div>
    <hr class="divider">
    <div>
      <div class="label-row"><span class="label-name">&#x110;&#x1ED9; cao MTF-01P</span><span class="label-val" id="sv-alt" style="font-size:22px;font-weight:800;color:var(--cyan)">-- m</span></div>
      <div class="prog-track"><div class="prog-fill" id="bar-alt" style="width:0;background:linear-gradient(90deg,var(--blue),var(--cyan))"></div></div>
      <div style="display:flex;justify-content:space-between;margin-top:4px;font-size:10px;color:#fff"><span>0 m</span><span id="alt-scale-label">Auto</span></div>

    </div>
  </div>

  <div class="card">
    <div class="card-title">La bàn</div>
    <div class="compass-panel">
      <div>
        <div class="compass-dial">
          <span class="compass-mark n">N</span>
          <span class="compass-mark e">E</span>
          <span class="compass-mark s">S</span>
          <span class="compass-mark w">W</span>
          <div class="compass-needle" id="compass-needle"></div>
        </div>
        <div class="heading-readout"><span id="heading-cardinal">--</span><b id="heading-value">--.-&deg;</b></div>
      </div>
    </div>
  </div>

  <div class="card full" style="padding:0; overflow:hidden; background:#111827">
    <div style="display:flex;justify-content:space-between;padding:10px 12px;border-bottom:1px solid #26324a">
      <b style="color:#93a4c7;font-size:14px;letter-spacing:1px;text-transform:uppercase">ĐỊNH VỊ GPS</b>
      <div style="display:flex;align-items:center;gap:8px">
        <span id="gps-fix-badge" class="badge badge-fail">NO FIX</span>
        <span id="gps-map-sat" style="color:#ff6b8a;font-weight:bold;">SAT: 0</span>
      </div>
    </div>
    <div style="padding: 0 12px 12px 12px">
      <div class="gps-map-shell" id="gps-map-shell" style="margin-top:12px;">
        <div id="gps-map"></div>
        <canvas id="gps-offline-map"></canvas>
        <div class="gps-map-overlay" id="gps-map-stat">GPS: --</div>
        <div class="gps-map-offline" id="gps-map-offline">Bản đồ cần Internet<br><small>Đang dùng bản đồ nội bộ</small></div>
        <div style="position:absolute; bottom:10px; right:10px; z-index:999;">
           <button class="btn btn-action" style="padding:6px 10px; background:rgba(0,0,0,0.6); border:none; border-radius:4px;" onclick="setGpsHome()">Set Home</button>
        </div>
      </div>
    </div>
  </div>

  <div class="card">
    <div class="card-title">Tay cầm (SBUS)</div>
    <div class="ch-rows">
      <div class="ch-row"><span class="ch-name">CH1 Roll</span><div class="bar-track"><div class="bar-fill" id="bar-ch1" style="width:50%"></div></div><span class="ch-val" id="val-ch1">1500</span></div>
      <div class="ch-row"><span class="ch-name">CH2 Pitch</span><div class="bar-track"><div class="bar-fill" id="bar-ch2" style="width:50%"></div></div><span class="ch-val" id="val-ch2">1500</span></div>
      <div class="ch-row"><span class="ch-name">CH3 Ga</span><div class="bar-track"><div class="bar-fill" id="bar-ch3" style="width:0;background:linear-gradient(90deg,var(--green),var(--cyan))"></div></div><span class="ch-val" id="val-ch3">1000</span></div>
      <div class="ch-row"><span class="ch-name">CH4 ARM</span><div class="bar-track"><div class="bar-fill" id="bar-ch4" style="width:50%;background:linear-gradient(90deg,var(--purple),var(--pink))"></div></div><span class="ch-val" id="val-ch4">1500</span></div>
      <div class="ch-row"><span class="ch-name">CH5 Aux1</span><div class="bar-track"><div class="bar-fill" id="bar-ch5" style="width:0;background:linear-gradient(90deg,var(--orange),var(--yellow))"></div></div><span class="ch-val" id="val-ch5">1000</span></div>
      <div class="ch-row"><span class="ch-name">CH6 Aux2</span><div class="bar-track"><div class="bar-fill" id="bar-ch6" style="width:0;background:linear-gradient(90deg,var(--orange),var(--yellow))"></div></div><span class="ch-val" id="val-ch6">1000</span></div>
    </div>
  </div>

  <div class="card">
    <div class="card-title">Xung Motor (&#xB5;s)</div>
    <div class="drone-wrap">
      <div class="drone-diagram">
        <div style="position:absolute;top:50%;left:50%;width:66px;height:2px;background:var(--border);transform-origin:left center;transform:rotate(-135deg)"></div>
        <div style="position:absolute;top:50%;left:50%;width:66px;height:2px;background:var(--border);transform-origin:left center;transform:rotate(-45deg)"></div>
        <div style="position:absolute;top:50%;left:50%;width:66px;height:2px;background:var(--border);transform-origin:left center;transform:rotate(135deg)"></div>
        <div style="position:absolute;top:50%;left:50%;width:66px;height:2px;background:var(--border);transform-origin:left center;transform:rotate(45deg)"></div>
        <div class="center-dot">F</div>
        <div class="motor-node tl" id="mn3"><span class="m-label">M3<span class="m-dir">&#x21BB;</span></span><span class="m-val" id="mv3">1000</span></div>
        <div class="motor-node tr" id="mn2"><span class="m-label">M2<span class="m-dir">&#x21BA;</span></span><span class="m-val" id="mv2">1000</span></div>
        <div class="motor-node bl" id="mn4"><span class="m-label">M4<span class="m-dir">&#x21BA;</span></span><span class="m-val" id="mv4">1000</span></div>
        <div class="motor-node br" id="mn1"><span class="m-label">M1<span class="m-dir">&#x21BB;</span></span><span class="m-val" id="mv1">1000</span></div>
      </div>
    </div>
    <div style="display:flex;flex-direction:column;gap:5px">
      <div class="ch-row"><span class="ch-name">M1 8045R</span><div class="bar-track"><div class="bar-fill" id="bm1" style="width:0%"></div></div><span class="ch-val" id="vm1">1000</span></div>
      <div class="ch-row"><span class="ch-name">M2 8045</span><div class="bar-track"><div class="bar-fill" id="bm2" style="width:0%"></div></div><span class="ch-val" id="vm2">1000</span></div>
      <div class="ch-row"><span class="ch-name">M3 8045R</span><div class="bar-track"><div class="bar-fill" id="bm3" style="width:0%"></div></div><span class="ch-val" id="vm3">1000</span></div>
      <div class="ch-row"><span class="ch-name">M4 8045</span><div class="bar-track"><div class="bar-fill" id="bm4" style="width:0%"></div></div><span class="ch-val" id="vm4">1000</span></div>
    </div>
  </div>

  <div class="card full">
    <div class="card-title">Điều khiển</div>
    <div class="btn-grid">
      <button class="btn btn-arm" id="btn-arm" onclick="doArm()">&#x2708; ARM</button>
      <button class="btn btn-disarm" id="btn-disarm" onclick="doDisarm()">&#x1F534; DISARM</button>
    </div>
    <div class="card-title">Hành động nhanh</div>
    <div class="btn-grid">
      <button class="btn btn-action" onclick="doSetCenter()">&#x1F3AF; Đặt tâm tay cầm</button>
    </div>
  </div>

  <div class="card full">
    <div class="card-title">Góc lệch IMU (Bù phẳng)</div>
    <div class="off-row">
      <span class="off-label" style="color:var(--red)">Roll</span>
      <div style="display:flex;align-items:center;gap:6px">
        <div class="off-btns">
          <button class="off-btn" onclick="adjOff('rOff',-0.1)">&#x2212;</button>
          <button class="off-btn" onclick="adjOff('rOff',+0.1)">&#x002B;</button>
        </div>
        <input class="off-input" id="off-roll" type="number" step="0.1" value="-3.0">
      </div>
      <span style="font-size:11px;color:#fff">deg</span>
    </div>
    <div class="off-row">
      <span class="off-label" style="color:var(--green)">Pitch</span>
      <div style="display:flex;align-items:center;gap:6px">
        <div class="off-btns">
          <button class="off-btn" onclick="adjOff('pOff',-0.1)">&#x2212;</button>
          <button class="off-btn" onclick="adjOff('pOff',+0.1)">&#x002B;</button>
        </div>
        <input class="off-input" id="off-pitch" type="number" step="0.1" value="0.7">
      </div>
      <span style="font-size:11px;color:#fff">deg</span>
    </div>
    <div style="display:flex;gap:10px;margin-top:14px">
      <button class="btn btn-action" style="flex:1;border-color:var(--cyan);color:var(--cyan)" onclick="saveOffset()">&#x1F4BE; Lưu góc lệch</button>
      <button class="btn btn-action" style="flex:1" onclick="resetOffset()">&#x21A9; Đặt về mặc định</button>
    </div>
  </div>

  <div class="card full">
    <div class="card-title">Cài đặt PID</div>
    <div class="pid-tune-grid">
      <div class="pid-tune-card" style="--axis:var(--teal)">
        <div class="pid-tune-head"><span class="pid-tune-name">Rate Roll & Pitch</span><span class="pid-tune-id">ID: 0,1</span></div>
        <div class="pid-live-grid">
          <div class="pid-live-box"><span class="pid-live-label">PV (đo)</span><span class="pid-live-val" id="pv-rr">0.00</span></div>
          <div class="pid-live-box"><span class="pid-live-label">SP (đặt)</span><span class="pid-live-val" id="sp-rr">0.00</span></div>
          <div class="pid-live-box"><span class="pid-live-label">Error</span><span class="pid-live-val err" id="er-rr">0.00</span></div>
        </div>
        <div class="pid-tune-row"><label>P</label><input class="pid-num" id="rrKp" type="number" step="0.01" value="1.15" oninput="syncTune('rrKp',this.value)"></div>
        <div class="pid-tune-row"><label>I</label><input class="pid-num" id="rrKi" type="number" step="0.001" value="0.08" oninput="syncTune('rrKi',this.value)"></div>
        <div class="pid-tune-row"><label>D</label><input class="pid-num" id="rrKd" type="number" step="0.0001" value="0.011" oninput="syncTune('rrKd',this.value)"></div>
        <button class="pid-update" onclick="savePID()">Cập nhật PID</button>
        <canvas class="pid-mini-chart" id="chart-rr"></canvas>
      </div>

      <div class="pid-tune-card" style="--axis:var(--yellow)">
        <div class="pid-tune-head"><span class="pid-tune-name">Yaw Rate</span><span class="pid-tune-id">ID: 2</span></div>
        <div class="pid-live-grid">
          <div class="pid-live-box"><span class="pid-live-label">PV (đo)</span><span class="pid-live-val" id="pv-yr">0.00</span></div>
          <div class="pid-live-box"><span class="pid-live-label">SP (đặt)</span><span class="pid-live-val" id="sp-yr">0.00</span></div>
          <div class="pid-live-box"><span class="pid-live-label">Error</span><span class="pid-live-val err" id="er-yr">0.00</span></div>
        </div>
        <div class="pid-tune-row"><label>P</label><input class="pid-num" id="yKp" type="number" step="0.1" value="0.8" oninput="syncTune('yKp',this.value)"></div>
        <div class="pid-tune-row"><label>I</label><input class="pid-num" id="yKi" type="number" step="0.001" value="0" oninput="syncTune('yKi',this.value)"></div>
        <div class="pid-tune-row"><label>D</label><input class="pid-num" id="yKd" type="number" step="0.001" value="0" oninput="syncTune('yKd',this.value)"></div>
        <button class="pid-update" onclick="savePID()">Cập nhật PID</button>
        <canvas class="pid-mini-chart" id="chart-yr"></canvas>
      </div>

      <div class="pid-tune-card" style="--axis:var(--blue)">
        <div class="pid-tune-head"><span class="pid-tune-name">Angle Roll & Pitch</span><span class="pid-tune-id">ID: 3,4</span></div>
        <div class="pid-live-grid">
          <div class="pid-live-box"><span class="pid-live-label">PV (đo)</span><span class="pid-live-val" id="pv-ar">0.00</span></div>
          <div class="pid-live-box"><span class="pid-live-label">SP (đặt)</span><span class="pid-live-val" id="sp-ar">0.00</span></div>
          <div class="pid-live-box"><span class="pid-live-label">Error</span><span class="pid-live-val err" id="er-ar">0.00</span></div>
        </div>
        <div class="pid-tune-row"><label>P</label><input class="pid-num" id="arKp" type="number" step="0.1" value="6" oninput="syncTune('arKp',this.value)"></div>
        <div class="pid-tune-row"><label>I</label><input class="pid-num" id="arKi" type="number" step="0.01" value="0" oninput="syncTune('arKi',this.value)"></div>
        <div class="pid-tune-row"><label>D</label><input class="pid-num" id="arKd" type="number" step="0.01" value="0" oninput="syncTune('arKd',this.value)"></div>
        <button class="pid-update" onclick="savePID()">Cập nhật PID</button>
        <canvas class="pid-mini-chart" id="chart-ar"></canvas>
      </div>

      <div class="pid-tune-card" style="--axis:var(--purple)">
        <div class="pid-tune-head"><span class="pid-tune-name">Altitude</span><span class="pid-tune-id">ID: 5</span></div>
        <div class="pid-live-grid">
          <div class="pid-live-box"><span class="pid-live-label">PV (đo)</span><span class="pid-live-val" id="pv-alt">0.00</span></div>
          <div class="pid-live-box"><span class="pid-live-label">SP (đặt)</span><span class="pid-live-val" id="sp-alt">0.00</span></div>
          <div class="pid-live-box"><span class="pid-live-label">Error</span><span class="pid-live-val err" id="er-alt">0.00</span></div>
        </div>
        <div class="pid-tune-row"><label>P</label><input class="pid-num" id="altKp" type="number" step="0.1" value="1" oninput="syncTune('altKp',this.value)"></div>
        <div class="pid-tune-row"><label>I</label><input class="pid-num" id="altKi" type="number" step="0.01" value="0" oninput="syncTune('altKi',this.value)"></div>
        <div class="pid-tune-row"><label>D</label><input class="pid-num" id="altKd" type="number" step="0.01" value="0" oninput="syncTune('altKd',this.value)"></div>
        <button class="pid-update" onclick="savePID()">Cập nhật PID</button>
        <canvas class="pid-mini-chart" id="chart-alt"></canvas>
      </div>
      <div class="pid-tune-card" style="--axis:#e67e22">
        <div class="pid-tune-head"><span class="pid-tune-name">Optical Flow XY</span><span class="pid-tune-id">ID: 6</span></div>
        <div class="pid-live-grid">
          <div class="pid-live-box"><span class="pid-live-label">PV (X/Y)</span><span class="pid-live-val" id="pv-opt">0.00</span></div>
          <div class="pid-live-box"><span class="pid-live-label">SP (đặt)</span><span class="pid-live-val" id="sp-opt">0.00</span></div>
          <div class="pid-live-box"><span class="pid-live-label">Error</span><span class="pid-live-val err" id="er-opt">0.00</span></div>
        </div>
        <div class="pid-tune-row"><label>P</label><input class="pid-num" id="optKp" type="number" step="0.01" value="0" oninput="syncTune('optKp',this.value)"></div>
        <div class="pid-tune-row"><label>I</label><input class="pid-num" id="optKi" type="number" step="0.01" value="0" oninput="syncTune('optKi',this.value)"></div>
        <div class="pid-tune-row"><label>D</label><input class="pid-num" id="optKd" type="number" step="0.01" value="0" oninput="syncTune('optKd',this.value)"></div>
        <div class="pid-tune-row"><label>Max deg</label><input class="pid-num" id="flowOutLimit" type="number" step="0.1" min="1" max="12" value="4" oninput="syncTune('flowOutLimit',this.value)"></div>
        <button class="pid-update" onclick="savePID()">Cập nhật PID</button>
        <canvas class="pid-mini-chart" id="chart-opt"></canvas>
      </div>
    </div>
    <div style="margin-top:12px;display:flex;gap:10px">
      <button class="btn btn-action" style="flex:1" onclick="resetPidDefault()">&#x21A9; Reset mặc định</button>
    </div>
  </div>

</div>
<div id="toast"></div>

<script>
"use strict";
let pidLoaded=false,altScale=10;
let lastGps=null;
let gpsHome={set:false,lat:0,lon:0};
let wasArmed=false;
let toastTimer=null;
let pidEditHoldUntil=0;
let gpsTrail=[];
let flowTrail=[];
let mapHeading=0;
let gpsMap=null,gpsMarker=null,gpsTrack=null,gpsFirstFix=true,gpsLeafletFailed=false,gpsTileErrors=0;
const tuneHistory={rr:[],pr:[],yr:[],ar:[],ap:[],alt:[]};
function showToast(msg,ok=true){
  const t=document.getElementById('toast');
  t.textContent=msg;t.className='show '+(ok?'ok':'err');
  clearTimeout(toastTimer);toastTimer=setTimeout(()=>t.className='',2500);
}
async function api(url){
  try{
    const sep=url.includes('?')?'&':'?';
    const r=await fetch(url+sep+'_='+Date.now(),{cache:'no-store'});
    const d=await r.json();
    if(d.msg)showToast(d.msg,d.ok!==false);
    else if(d.ok===false)showToast('Lỗi!',false);
    else showToast('OK \u2713',true);
    setTimeout(fetchStatus,60);return d;
  }catch(e){showToast('Mất kết nối!',false);return null;}
}
function doArm(){api('/arm');}
function doDisarm(){api('/disarm');}
function doSetCenter(){api('/set_center');}
async function setGpsHome(){
  if(!lastGps||!lastGps.fix){showToast('GPS chưa có fix',false);return;}
  gpsHome={set:true,lat:lastGps.lat,lon:lastGps.lon};
  drawGpsMap(lastGps);
  await api('/set_gps_home');
}
async function resetFlowMapWeb(){
  flowTrail=[];
  drawFlowMap(null);
  await api('/reset_flow_map');
}
function tuneVal(key){const el=id(key);return el?el.value:'0';}
async function savePID(){
  const keys=['rrKp','rrKi','rrKd','arKp','arKi','arKd','yKp','yKi','yKd','altKp','altKi','altKd','optKp','optKi','optKd','flowOutLimit'];
  let url='/save_pid?'+keys.map(k=>k+'='+encodeURIComponent(tuneVal(k))).join('&');
  url+='&prKp='+encodeURIComponent(tuneVal('rrKp'))+'&prKi='+encodeURIComponent(tuneVal('rrKi'))+'&prKd='+encodeURIComponent(tuneVal('rrKd'));
  url+='&apKp='+encodeURIComponent(tuneVal('arKp'))+'&apKi='+encodeURIComponent(tuneVal('arKi'))+'&apKd='+encodeURIComponent(tuneVal('arKd'));
  await api(url);
  pidEditHoldUntil=0;
}
function adjOff(key,delta){
  const el=id(key==='rOff'?'off-roll':'off-pitch');
  let v=Math.round((parseFloat(el.value||0)+delta)*10)/10;
  v=Math.max(-30,Math.min(30,v));
  el.value=v.toFixed(1);
}
function saveOffset(){
  const r=parseFloat(id('off-roll').value||0);
  const p=parseFloat(id('off-pitch').value||0);
  api('/save_offset?rOff='+r.toFixed(2)+'&pOff='+p.toFixed(2));
}
function resetOffset(){
  id('off-roll').value='-3.0';
  id('off-pitch').value='0.7';
  api('/save_offset?rOff=-3.00&pOff=0.70');
}

const STATUS_INTERVAL_MS = 180;

document.addEventListener('DOMContentLoaded',()=>drawFlowMap(null));

function resetPidDefault(){
  pidEditHoldUntil=0;
  setTuneValue('rrKp',1.15);setTuneValue('rrKi',0.08);setTuneValue('rrKd',0.011);
  setTuneValue('prKp',1.15);setTuneValue('prKi',0.08);setTuneValue('prKd',0.011);
  setTuneValue('arKp',6.0);setTuneValue('arKi',0);setTuneValue('arKd',0);
  setTuneValue('apKp',6.0);setTuneValue('apKi',0);setTuneValue('apKd',0);
  setTuneValue('yKp',0.8);setTuneValue('yKi',0);setTuneValue('yKd',0);
  setTuneValue('altKp',1);setTuneValue('altKi',0);setTuneValue('altKd',0);
  setTuneValue('optKp',0);setTuneValue('optKi',0);setTuneValue('optKd',0);setTuneValue('flowOutLimit',4);
  savePID();showToast('Reset mặc định PID \u2713',true);
}
function id(x){return document.getElementById(x);}
function syncTune(key,value){
  const input=id(key);
  if(input&&input.value!==String(value))input.value=value;
  pidEditHoldUntil=Date.now()+60000;
}
function setTuneValue(key,value){
  const input=id(key);
  if(Date.now()<pidEditHoldUntil)return;
  if(input&&document.activeElement===input)return;
  const text=Number.isFinite(parseFloat(value))?parseFloat(value).toString():'0';
  if(input)input.value=text;
}
function updatePidParamInputs(d){
  if(!(d.pRP||d.pRateR))return;
  const rateR=d.pRateR||d.pRP||[1.15,0.08,0.011];
  const rateP=d.pRateP||d.pRP||[1.15,0.08,0.011];
  const angleR=d.pAngleR||[6,0,0];
  const angleP=d.pAngleP||[6,0,0];
  setTuneValue('rrKp',parseFloat(rateR[0]).toFixed(3));
  setTuneValue('rrKi',parseFloat(rateR[1]).toFixed(4));
  setTuneValue('rrKd',parseFloat(rateR[2]).toFixed(4));
  setTuneValue('prKp',parseFloat(rateP[0]).toFixed(3));
  setTuneValue('prKi',parseFloat(rateP[1]).toFixed(4));
  setTuneValue('prKd',parseFloat(rateP[2]).toFixed(4));
  setTuneValue('arKp',parseFloat(angleR[0]).toFixed(3));
  setTuneValue('arKi',parseFloat(angleR[1]).toFixed(4));
  setTuneValue('arKd',parseFloat(angleR[2]).toFixed(4));
  setTuneValue('apKp',parseFloat(angleP[0]).toFixed(3));
  setTuneValue('apKi',parseFloat(angleP[1]).toFixed(4));
  setTuneValue('apKd',parseFloat(angleP[2]).toFixed(4));
  if(d.pYaw){
    setTuneValue('yKp',parseFloat(d.pYaw[0]).toFixed(3));
    setTuneValue('yKi',parseFloat(d.pYaw[1]).toFixed(4));
    setTuneValue('yKd',parseFloat(d.pYaw[2]).toFixed(4));
  }
  if(d.pAlt){
    setTuneValue('altKp',parseFloat(d.pAlt[0]).toFixed(3));
    setTuneValue('altKi',parseFloat(d.pAlt[1]).toFixed(3));
    setTuneValue('altKd',parseFloat(d.pAlt[2]).toFixed(3));
  }
  if(d.pOpt){
    setTuneValue('optKp',parseFloat(d.pOpt[0]).toFixed(3));
    setTuneValue('optKi',parseFloat(d.pOpt[1]).toFixed(3));
    setTuneValue('optKd',parseFloat(d.pOpt[2]).toFixed(3));
  }
  if(d.flowOutLimit!==undefined)setTuneValue('flowOutLimit',parseFloat(d.flowOutLimit).toFixed(1));
}
function pct(v){return Math.round((v-1000)/1000*100);}
const N=150;
let wR=Array(N).fill(0),wP=Array(N).fill(0),wOptX=Array(N).fill(0),wOptY=Array(N).fill(0);
function drawChart(cid,datasets,range){
  const cv=id(cid);if(!cv)return;
  const ctx=cv.getContext('2d'),W=cv.width,H=cv.height;
  ctx.clearRect(0,0,W,H);
  ctx.strokeStyle='#252540';ctx.lineWidth=1;
  ctx.beginPath();ctx.moveTo(0,H/2);ctx.lineTo(W,H/2);ctx.stroke();
  [0.25,0.75].forEach(f=>{ctx.strokeStyle='#1a1a30';ctx.lineWidth=1;ctx.beginPath();ctx.moveTo(0,f*H);ctx.lineTo(W,f*H);ctx.stroke();});
  datasets.forEach(({data,color})=>{
    ctx.strokeStyle=color;ctx.lineWidth=2;ctx.shadowColor=color;ctx.shadowBlur=5;
    ctx.beginPath();
    for(let i=0;i<N;i++){const x=(i/(N-1))*W,y=H/2-(data[i]/range)*(H/2-3);i===0?ctx.moveTo(x,y):ctx.lineTo(x,y);}
    ctx.stroke();ctx.shadowBlur=0;
  });
}
function drawOptChart(cid, dx, dy, range){
  const cv=id(cid);if(!cv)return;
  const c=prepCanvas(cv),ctx=c.ctx,W=c.w,H=c.h,dpr=c.dpr;
  ctx.clearRect(0,0,W,H);
  if(dx.length<2)return;
  ctx.lineWidth=2*dpr;
  ctx.strokeStyle='#f38ba8';
  ctx.beginPath();
  for(let i=0;i<N;i++){const x=(i/(N-1))*W, y=H/2-(dx[i]/range)*(H/2-3);i===0?ctx.moveTo(x,y):ctx.lineTo(x,y);}
  ctx.stroke();
  ctx.strokeStyle='#a6e3a1';
  ctx.beginPath();
  for(let i=0;i<N;i++){const x=(i/(N-1))*W, y=H/2-(dy[i]/range)*(H/2-3);i===0?ctx.moveTo(x,y):ctx.lineTo(x,y);}
  ctx.stroke();
}
let lastPlotlyUpdate = 0;
function rememberFlowPoint(x,y,armed){
  const last=flowTrail[flowTrail.length-1];
  if(!last||Math.abs(last.x-x)>0.05||Math.abs(last.y-y)>0.05)flowTrail.push({x,y});
  if(flowTrail.length>150)flowTrail.shift();
}
function drawFlowMap(d){
  const cv=id('flow-map');if(!cv)return;
  const pos=d&&d.optPosXY?[finite(d.optPosXY[0]),finite(d.optPosXY[1])]:[0,0];
  const vel=d&&d.optXY?[finite(d.optXY[0])/10.0,finite(d.optXY[1])/10.0]:[0,0];
  const armed=!!(d&&d.daKichHoat);
  const q=parseInt((d&&d.flowQuality!==undefined)?d.flowQuality:0,10)||0;
  const age=parseInt((d&&d.flowAgeMs!==undefined)?d.flowAgeMs:0,10)||0;
  rememberFlowPoint(pos[0],pos[1],armed);

  const flowLive=q>10&&age<600;
  const live=armed&&flowLive;
  const badge=id('flow-map-badge');
  if(badge){badge.textContent=live?'FLOW OK':(flowLive?'FLOW DATA':(armed?'FLOW WAIT':'FLOW OFF'));badge.className='badge '+(flowLive?'badge-ok':'badge-fail');}
  if(id('flow-pos-x'))id('flow-pos-x').textContent=pos[0].toFixed(1)+' cm';
  if(id('flow-pos-y'))id('flow-pos-y').textContent=pos[1].toFixed(1)+' cm';
  if(id('flow-vel-xy'))id('flow-vel-xy').textContent=vel[0].toFixed(1)+' / '+vel[1].toFixed(1)+' cm/s';
  if(id('flow-q-age'))id('flow-q-age').textContent='Q '+q+' / '+age+' ms';

  if(!window.Plotly) return;
  const now = Date.now();
  if(now - lastPlotlyUpdate < 250 && flowTrail.length > 0) return;
  lastPlotlyUpdate = now;

  let tx=[], ty=[];
  flowTrail.forEach(p => { tx.push(p.x); ty.push(p.y); });

  const traceTrail = {
    x: tx, y: ty,
    mode: 'lines',
    type: 'scatter',
    name: 'Quỹ đạo',
    line: { color: '#89b4fa', width: 2 }
  };
  const traceCurrent = {
    x: [pos[0]], y: [pos[1]],
    mode: 'markers',
    type: 'scatter',
    name: 'Drone hiện tại',
    marker: { color: '#f38ba8', size: 10 }
  };
  const traceTarget = {
    x: [0], y: [0],
    mode: 'markers',
    type: 'scatter',
    name: 'Setpoint',
    marker: { color: '#a6e3a1', symbol: 'cross', size: 12 }
  };

  const maxAbs = Math.max(50, ...tx.map(Math.abs), ...ty.map(Math.abs), Math.abs(pos[0]), Math.abs(pos[1]));
  const rangeMax = Math.ceil(maxAbs / 50) * 50;
  const layout = {
    title: { text: 'Trực quan hóa vị trí & Setpoint Drone', font: {color: '#93a4c7', size: 12} },
    margin: { l: 40, r: 20, b: 40, t: 30 },
    paper_bgcolor: 'rgba(0,0,0,0)',
    plot_bgcolor: 'rgba(0,0,0,0)',
    xaxis: { title: 'Trục X (cm)', color: '#93a4c7', gridcolor: '#26324a', zerolinecolor: '#4a4a6a', scaleanchor: 'y', scaleratio: 1, range: [-rangeMax, rangeMax] },
    yaxis: { title: 'Trục Y (cm)', color: '#93a4c7', gridcolor: '#26324a', zerolinecolor: '#4a4a6a', range: [-rangeMax, rangeMax] },
    legend: { x: 0, y: 1, font: {color: '#93a4c7', size: 10}, bgcolor: 'rgba(0,0,0,0)' },
    showlegend: true
  };

  Plotly.react(cv, [traceTrail, traceCurrent, traceTarget], layout, {displayModeBar: false});
}
function prepCanvas(cv){
  const dpr=window.devicePixelRatio||1;
  const rect=cv.getBoundingClientRect();
  const w=Math.max(180,Math.round(rect.width*dpr));
  const h=Math.max(80,Math.round(rect.height*dpr));
  if(cv.width!==w||cv.height!==h){cv.width=w;cv.height=h;}
  return {ctx:cv.getContext('2d'),w,h,dpr};
}
function pushTuneHistory(name,value){
  const a=tuneHistory[name];if(!a)return;
  a.push(finite(value));while(a.length>80)a.shift();
}
function drawMiniChart(cid,name,range,color){
  const cv=id(cid);if(!cv)return;
  const c=prepCanvas(cv),ctx=c.ctx,W=c.w,H=c.h,dpr=c.dpr,data=tuneHistory[name]||[];
  ctx.clearRect(0,0,W,H);
  ctx.fillStyle='#121424';ctx.fillRect(0,0,W,H);
  ctx.strokeStyle='#252840';ctx.lineWidth=1*dpr;
  for(let i=1;i<4;i++){const y=i*H/4;ctx.beginPath();ctx.moveTo(0,y);ctx.lineTo(W,y);ctx.stroke();}
  ctx.strokeStyle='rgba(255,255,255,.18)';ctx.setLineDash([3*dpr,3*dpr]);
  ctx.beginPath();ctx.moveTo(0,H/2);ctx.lineTo(W,H/2);ctx.stroke();ctx.setLineDash([]);
  if(data.length<2)return;
  ctx.strokeStyle=color;ctx.lineWidth=2*dpr;ctx.beginPath();
  data.forEach((v,i)=>{
    const x=(i/79)*W;
    const y=H/2-(Math.max(-range,Math.min(range,v))/range)*(H/2-6*dpr);
    i?ctx.lineTo(x,y):ctx.moveTo(x,y);
  });
  ctx.stroke();
}
function setTuneLive(name,pv,sp,range,color){
  const el=id('pv-'+name);
  if(!el) return;
  const err=finite(sp)-finite(pv);
  el.textContent=finite(pv).toFixed(2);
  id('sp-'+name).textContent=finite(sp).toFixed(2);
  id('er-'+name).textContent=err.toFixed(2);
  pushTuneHistory(name,pv);
  drawMiniChart('chart-'+name,name,range,color);
}
function clampValue(v,min,max){return Math.max(min,Math.min(max,v));}
function lerp(a,b,t){return a+(b-a)*t;}
function smoothValue(current,target,alpha){return lerp(current,target,alpha);}
function smoothAngle(current,target,alpha){
  const diff=((target-current+540)%360)-180;
  return current+diff*alpha;
}

const VISUAL_ANGLE_LIMIT=45;
let droneEls=null;
let droneTarget={roll:0,pitch:0,yaw:0,throttle:1000,armed:false,motors:[1000,1000,1000,1000]};
let droneVisual={roll:0,pitch:0,yaw:0,throttle:1000,motors:[1000,1000,1000,1000]};

function getDroneEls(){
  if(droneEls)return droneEls;
  droneEls={
    body:id('drone-3d'),
    shadow:id('drone-shadow'),
    props:[id('d3-prop1'),id('d3-prop2'),id('d3-prop3'),id('d3-prop4')],
    motors:[id('d3-motor1'),id('d3-motor2'),id('d3-motor3'),id('d3-motor4')]
  };
  return droneEls;
}

function updateDroneVisual(roll,pitch,yaw,throttle,armed){
  droneTarget.roll=clampValue(parseFloat(roll)||0,-VISUAL_ANGLE_LIMIT,VISUAL_ANGLE_LIMIT);
  droneTarget.pitch=clampValue(parseFloat(pitch)||0,-VISUAL_ANGLE_LIMIT,VISUAL_ANGLE_LIMIT);
  droneTarget.yaw=parseFloat(yaw)||0;
  droneTarget.throttle=clampValue(parseFloat(throttle)||1000,1000,2000);
  droneTarget.armed=!!armed;
}

function setDroneMotorTargets(motors){
  for(let i=0;i<4;i++)droneTarget.motors[i]=clampValue(parseInt((motors||[])[i]||1000,10),1000,2000);
}

function applyPropVisual(el,motorEl,us,armed){
  if(!el)return;
  const v=clampValue(parseFloat(us)||1000,1000,2000);
  if(!armed||v<=1030){
    el.classList.remove('spinning');
    el.style.setProperty('--spin-dur','1.8s');
    el.style.setProperty('--prop-opacity','.22');
    el.style.setProperty('--prop-blur','0px');
    if(motorEl)motorEl.style.setProperty('--motor-glow','12px');
    return;
  }
  const t=clampValue((v-1030)/970,0,1);
  const dur=0.92-t*0.80;
  el.style.setProperty('--spin-dur',Math.max(0.08,dur).toFixed(3)+'s');
  el.style.setProperty('--prop-opacity',(0.50+t*0.44).toFixed(2));
  el.style.setProperty('--prop-blur',(t*1.15).toFixed(2)+'px');
  el.classList.add('spinning');
  if(motorEl)motorEl.style.setProperty('--motor-glow',Math.round(12+t*24)+'px');
}

function animateDroneVisual(ts){
  const e=getDroneEls();
  const active=droneTarget.armed&&droneTarget.throttle>1030;
  droneVisual.roll=smoothValue(droneVisual.roll,droneTarget.roll,0.14);
  droneVisual.pitch=smoothValue(droneVisual.pitch,droneTarget.pitch,0.14);
  droneVisual.yaw=smoothAngle(droneVisual.yaw,droneTarget.yaw,0.10);
  droneVisual.throttle=smoothValue(droneVisual.throttle,droneTarget.throttle,0.16);
  for(let i=0;i<4;i++)droneVisual.motors[i]=smoothValue(droneVisual.motors[i],droneTarget.motors[i],0.18);

  const lift=active?clampValue((droneVisual.throttle-1030)/970,0,1):0;
  const hover=active?Math.sin(ts*0.006)*1.8*lift:0;
  if(e.body){
    e.body.style.transform=
      `translate3d(-50%,-50%,0) translateY(${hover.toFixed(2)}px) `+
      `rotateZ(${droneVisual.yaw.toFixed(2)}deg) `+
      `rotateX(${droneVisual.pitch.toFixed(2)}deg) `+
      `rotateY(${droneVisual.roll.toFixed(2)}deg)`;
  }
  if(e.shadow){
    const sx=1+Math.abs(droneVisual.roll)/90+lift*0.10;
    const sy=1+Math.abs(droneVisual.pitch)/120;
    const dx=droneVisual.roll*0.32;
    const dy=-droneVisual.pitch*0.22;
    e.shadow.style.opacity=(0.30+lift*0.22).toFixed(2);
    e.shadow.style.transform=`translate(calc(-50% + ${dx.toFixed(1)}px),calc(-50% + ${dy.toFixed(1)}px)) translateZ(-36px) rotateZ(${droneVisual.yaw.toFixed(1)}deg) scale(${sx.toFixed(2)},${sy.toFixed(2)})`;
  }
  for(let i=0;i<4;i++)applyPropVisual(e.props[i],e.motors[i],droneVisual.motors[i],droneTarget.armed);
  requestAnimationFrame(animateDroneVisual);
}

function setMotorColor(nid,us){
  const nd=id(nid),t=Math.max(0,Math.min(1,(us-1000)/1000));
  const r=Math.round(26+t*229),g2=Math.round(220-t*123),b=Math.round(235-t*227);
  nd.style.borderColor=t>0.05?`rgb(${r},${g2},${b})`:'var(--border)';
  nd.style.boxShadow=t>0.05?`0 0 ${Math.round(t*16)}px rgba(${r},${g2},${b},0.5)`:'none';
}
function fmtTemp(v,ok=true){
  const n=parseFloat(v);
  return ok&&Number.isFinite(n)?Math.round(n)+'\u00b0C':'--\u00b0C';
}
function setSignal(chipId,barsId,valId,value,enabled=true){
  const chip=id(chipId),bars=id(barsId).children;
  const pctVal=enabled?Math.max(0,Math.min(100,parseFloat(value)||0)):0;
  const on=pctVal>0?Math.max(1,Math.ceil(pctVal/25)):0;
  chip.className='signal-chip '+(!enabled||pctVal<=0?'fail':(pctVal<35?'warn':'ok'));
  for(let i=0;i<bars.length;i++)bars[i].className=i<on?'on':'';
  id(valId).textContent=Math.round(pctVal)+'%';
}
function finite(v){const n=parseFloat(v);return Number.isFinite(n)?n:0;}
function wrap360(v){return ((finite(v)%360)+360)%360;}
function headingName(deg){
  const dirs=['N','NE','E','SE','S','SW','W','NW'];
  return dirs[Math.round((((deg%360)+360)%360)/45)%8];
}
function gpsHasFix(d){
  const lat=parseFloat(d.gpsLat),lon=parseFloat(d.gpsLon);
  return !!(d.gpsOk&&d.gpsFix&&Number.isFinite(lat)&&Number.isFinite(lon)&&(Math.abs(lat)>0.000001||Math.abs(lon)>0.000001));
}
function gpsHasData(d){
  return !!(d.gpsOk||finite(d.gpsBytes)>0||parseInt(d.gpsSat||0,10)>0||parseInt(d.gpsSats||0,10)>0);
}
function updateCompassUI(d){
  const ok=!!d.compassOk,h=finite(d.headingDeg);
  id('compass-needle').style.transform='translate(-50%,-100%) rotate('+h.toFixed(1)+'deg)';
  id('heading-value').textContent=ok?h.toFixed(1)+'\u00b0':'--.-\u00b0';
  id('heading-cardinal').textContent=ok?headingName(h):'--';
}
function updateGpsUI(d){
  const data=gpsHasData(d);
  const rawFix=!!(d.gpsOk&&d.gpsFix);
  const fix=gpsHasFix(d);
  const g={
    fix,
    lat:finite(d.gpsLat),
    lon:finite(d.gpsLon),
    sat:parseInt(d.gpsSat||0,10),
    hdop:finite(d.gpsHdop),
    alt:finite(d.gpsAltM),
    speed:finite(d.gpsSpeedKmh),
    course:finite(d.gpsCourseDeg)
  };
  lastGps=g;
  mapHeading=d.compassOk?finite(d.headingDeg):finite(d.gocYaw);
  if(d.gpsHomeSet){
    gpsHome={set:true,lat:finite(d.gpsHomeLat),lon:finite(d.gpsHomeLon)};
  }
  const gsat=id('gps-map-sat');
  if(gsat) gsat.textContent='SAT: '+g.sat;
  const gb=id('gps-fix-badge');
  if(gb){gb.textContent=fix?'FIX OK':(rawFix?'FIX WAIT POS':(data?'GPS DATA':'NO FIX'));gb.className='badge '+(data?'badge-ok':'badge-fail');}
  drawGpsMap(g);
}
function gpsMetersFrom(baseLat,baseLon,lat,lon){
  const mLat=111320;
  const mLon=111320*Math.cos(baseLat*Math.PI/180);
  return {x:(lon-baseLon)*mLon,y:(lat-baseLat)*mLat};
}
function rememberGpsPoint(g){
  if(!g||!g.fix)return;
  const last=gpsTrail[gpsTrail.length-1];
  if(!last||Math.abs(last.lat-g.lat)>0.000001||Math.abs(last.lon-g.lon)>0.000001)gpsTrail.push({lat:g.lat,lon:g.lon});
  if(gpsTrail.length>160)gpsTrail.shift();
}
function setGpsMapMode(offline){
  const shell=id('gps-map-shell');if(shell)shell.classList.toggle('offline-on',!!offline);
}
function makeGpsIcon(){
  const h=((mapHeading%360)+360)%360;
  return L.divIcon({
    className:'custom-pin',
    html:'<div class="gps-drone-pin"><span class="gps-pin-arrow" style="transform:rotate('+h.toFixed(1)+'deg)"></span><span class="gps-pin-dot"></span></div>',
    iconSize:[28,28],
    iconAnchor:[14,14]
  });
}
function initGpsLeaflet(){
  if(gpsMap)return true;
  if(gpsLeafletFailed||typeof L==='undefined')return false;
  try{
    gpsMap=L.map('gps-map',{zoomControl:true,attributionControl:true}).setView([10.762622,106.660172],15);
    const tiles=L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png',{
      maxZoom:19,
      attribution:'© OpenStreetMap'
    }).addTo(gpsMap);
    tiles.on('tileerror',()=>{
      gpsTileErrors++;
      if(gpsTileErrors>6){
        gpsLeafletFailed=true;
        setGpsMapMode(true);
        drawOfflineGpsMap(lastGps);
      }
    });
    gpsMarker=L.marker([10.762622,106.660172],{icon:makeGpsIcon()}).addTo(gpsMap);
    gpsTrack=L.polyline([],{color:'#12b886',weight:3,opacity:.75}).addTo(gpsMap);
    setTimeout(()=>gpsMap.invalidateSize(),80);
    return true;
  }catch(e){
    gpsLeafletFailed=true;
    return false;
  }
}
function drawLeafletGpsMap(g){
  const hasFix=!!(g&&g.fix);
  setGpsMapMode(false);
  if(!gpsMap)return;
  setTimeout(()=>gpsMap.invalidateSize(),0);
  if(gpsTrack)gpsTrack.setLatLngs(gpsTrail.map(pt=>[pt.lat,pt.lon]));
  if(gpsHome.set){
    const home=[gpsHome.lat,gpsHome.lon];
    if(!window.gpsHomeMarker)window.gpsHomeMarker=L.circleMarker(home,{radius:6,color:'#ffffff',weight:2,fillColor:'#a6e3a1',fillOpacity:.9}).addTo(gpsMap).bindTooltip('HOME');
    else window.gpsHomeMarker.setLatLng(home);
  }
  if(!hasFix){
    id('gps-map-stat').textContent=g&&g.sat?'GPS DATA, WAIT FIX':'WAITING GPS';
    return;
  }
  const pos=[g.lat,g.lon];
  if(gpsMarker){gpsMarker.setLatLng(pos);gpsMarker.setIcon(makeGpsIcon());}
  if(gpsFirstFix){gpsMap.setView(pos,16);gpsFirstFix=false;}
  else gpsMap.panTo(pos,{animate:true,duration:.2});
  id('gps-map-stat').textContent='GPS '+g.lat.toFixed(6)+', '+g.lon.toFixed(6)+' | H '+mapHeading.toFixed(0)+'°';
}
function drawGpsMap(g){
  rememberGpsPoint(g);
  if(initGpsLeaflet()&&!gpsLeafletFailed)drawLeafletGpsMap(g);
  else drawOfflineGpsMap(g);
}
function drawOfflineGpsMap(g){
  const cv=id('gps-offline-map');if(!cv)return;
  setGpsMapMode(true);
  const c=prepCanvas(cv),ctx=c.ctx,W=c.w,H=c.h,dpr=c.dpr;
  const cx=W/2,cy=H/2,hasFix=!!(g&&g.fix);
  ctx.clearRect(0,0,W,H);
  const bg=ctx.createLinearGradient(0,0,W,H);
  bg.addColorStop(0,'#08101d');bg.addColorStop(1,'#03060c');
  ctx.fillStyle=bg;ctx.fillRect(0,0,W,H);
  ctx.strokeStyle='#16243a';ctx.lineWidth=1*dpr;
  for(let i=0;i<=10;i++){
    const x=i*W/10,y=i*H/10;
    ctx.beginPath();ctx.moveTo(x,0);ctx.lineTo(x,H);ctx.stroke();
    ctx.beginPath();ctx.moveTo(0,y);ctx.lineTo(W,y);ctx.stroke();
  }
  ctx.strokeStyle='#263a5e';
  ctx.beginPath();ctx.moveTo(cx,0);ctx.lineTo(cx,H);ctx.moveTo(0,cy);ctx.lineTo(W,cy);ctx.stroke();
  ctx.fillStyle='#93a4c7';ctx.font=(12*dpr)+'px system-ui';ctx.fillText('N',cx+8*dpr,18*dpr);

  const baseLat=gpsHome.set?gpsHome.lat:(hasFix?g.lat:0);
  const baseLon=gpsHome.set?gpsHome.lon:(hasFix?g.lon:0);
  if(!gpsHome.set&&!hasFix){
    id('gps-map-stat').textContent=g&&g.sat?'GPS DATA, WAIT FIX':'WAITING GPS';
    ctx.fillStyle='#93a4c7';ctx.textAlign='center';ctx.font=(16*dpr)+'px system-ui';
    ctx.fillText(g&&g.sat?'GPS DATA - NO FIX':'WAITING GPS',cx,cy);
    ctx.textAlign='left';return;
  }

  let maxMeters=8;
  const relDrone=hasFix?gpsMetersFrom(baseLat,baseLon,g.lat,g.lon):{x:0,y:0};
  maxMeters=Math.max(maxMeters,Math.abs(relDrone.x),Math.abs(relDrone.y));
  gpsTrail.forEach(pt=>{const r=gpsMetersFrom(baseLat,baseLon,pt.lat,pt.lon);maxMeters=Math.max(maxMeters,Math.abs(r.x),Math.abs(r.y));});
  const ranges=[10,20,50,100,200,500,1000,2000];
  let range=ranges[ranges.length-1];
  for(const r of ranges){if(maxMeters*1.35<=r){range=r;break;}}
  const ppm=Math.min(W,H)*0.42/range;
  ctx.strokeStyle='#21304d';ctx.fillStyle='#5f6f96';ctx.textAlign='center';ctx.font=(10*dpr)+'px system-ui';
  for(let m=-range;m<=range;m+=range/4){
    const x=cx+m*ppm,y=cy-m*ppm;
    ctx.beginPath();ctx.moveTo(x,0);ctx.lineTo(x,H);ctx.stroke();
    ctx.beginPath();ctx.moveTo(0,y);ctx.lineTo(W,y);ctx.stroke();
    if(m!==0)ctx.fillText(Math.round(m)+'m',x,cy-5*dpr);
  }

  function screenPos(lat,lon){const r=gpsMetersFrom(baseLat,baseLon,lat,lon);return {x:cx+r.x*ppm,y:cy-r.y*ppm};}
  if(gpsTrail.length>1){
    ctx.strokeStyle='#12b886';ctx.globalAlpha=.75;ctx.lineWidth=2*dpr;ctx.beginPath();
    gpsTrail.forEach((pt,i)=>{const p=screenPos(pt.lat,pt.lon);i?ctx.lineTo(p.x,p.y):ctx.moveTo(p.x,p.y);});
    ctx.stroke();ctx.globalAlpha=1;
  }
  if(gpsHome.set){
    ctx.fillStyle='#a6e3a1';ctx.beginPath();ctx.arc(cx,cy,5*dpr,0,Math.PI*2);ctx.fill();
    ctx.fillText('HOME',cx,cy-10*dpr);
  }
  if(hasFix){
    const p=screenPos(g.lat,g.lon);
    ctx.save();ctx.translate(p.x,p.y);ctx.rotate(mapHeading*Math.PI/180);
    ctx.fillStyle='#ff6b8a';ctx.strokeStyle='#ffffff';ctx.lineWidth=1.5*dpr;
    ctx.beginPath();ctx.moveTo(0,-12*dpr);ctx.lineTo(8*dpr,10*dpr);ctx.lineTo(0,5*dpr);ctx.lineTo(-8*dpr,10*dpr);ctx.closePath();ctx.fill();ctx.stroke();
    ctx.restore();
    ctx.fillStyle='#ff6b8a';ctx.fillText('DRONE',p.x,p.y-16*dpr);
  }
  ctx.textAlign='left';
  id('gps-map-stat').textContent=hasFix?('GPS '+g.lat.toFixed(6)+', '+g.lon.toFixed(6)+' | H '+mapHeading.toFixed(0)+'° | R '+range+'m'):(gpsHome.set?'HOME SET, WAIT GPS':'WAITING GPS');
}
function updateUI(d){
  if(d.daKichHoat){
    if(!wasArmed){ flowTrail=[]; wasArmed=true; }
    id('hdr-arm').textContent='\u2708 ARMED';id('hdr-arm').className='badge badge-armed';
  } else {
    wasArmed=false;
    id('hdr-arm').textContent='DISARMED';id('hdr-arm').className='badge badge-disarmed';
  }
  function setB(el,on){el.className='badge '+(on?'badge-ok':'badge-fail');}
  const sbusLive=!!(d.sbusOk&&!d.matTinHieu&&finite(d.mucSbus)>0);
  const gpsFix=!!(d.gpsOk&&d.gpsFix);
  const gpsData=gpsHasData(d);
  const imuScore=finite(d.imuScore),gpsScore=finite(d.gpsScore),compassScore=finite(d.compassScore),mtfScore=finite(d.mtfScore),flowScore=finite(d.flowScore);
  const imuTrusted=!!(d.icmOk&&imuScore>=35);
  setB(id('b-pid'),d.batPid);setB(id('b-icm'),imuTrusted);setB(id('b-alt'),!!(d.mtfTrusted||d.altSensorOk));setB(id('b-gps'),!!(d.gpsTrusted||gpsFix||gpsData));setB(id('b-compass'),!!(d.compassTrusted||d.compassOk));setB(id('b-sbus'),sbusLive);
  id('b-icm').lastElementChild.textContent=imuTrusted?'MPU '+imuScore.toFixed(0):'MPU BAD';
  id('b-alt').lastElementChild.textContent=(d.mtfTrusted?'MTF ':'ALT ')+(d.mtfTrusted?'OK':'OFF');
  id('b-gps').lastElementChild.textContent=d.gpsTrusted?'GPS OK':(gpsFix?'GPS FIX':(gpsData?'GPS DATA':'GPS OFF'));
  id('b-compass').lastElementChild.textContent=d.compassTrusted?'QMC OK':(d.compassOk?'QMC DATA':'QMC FAIL');
  id('b-sbus').lastElementChild.textContent=sbusLive?'SBUS':'SBUS OFF';
  setSignal('sig-wifi','bars-wifi','sig-wifi-val',d.mucWifi,true);
  setSignal('sig-sbus','bars-sbus','sig-sbus-val',d.mucSbus,sbusLive);
  id('b-fs').style.display=d.matTinHieu?'':'none';
  const bw=id('b-batt-warn');
  if(d.pinCan||d.pinYeu){bw.style.display='';bw.className='badge badge-fail';id('t-batt-warn').textContent=d.pinCan?'PIN CẠN':'PIN YẾU';}
  else{bw.style.display='none';}
  id('b-autoland').style.display=d.tuDongHaCanh?'':'none';
  const battV=parseFloat(d.dienApPin||0),battPct=parseFloat(d.phanTramPin||0);
  const hb=id('hdr-batt');
  hb.textContent='\uD83D\uDD0B '+battV.toFixed(2)+'V \u00b7 '+battPct.toFixed(0)+'%';
  hb.className='hdr-batt '+(d.pinCan?'fail':(d.pinYeu?'warn':''));
  id('temp-mpu').textContent=fmtTemp(d.nhietMpu,d.icmOk);
  id('temp-esp').textContent=fmtTemp(d.nhietEsp,true);
  const r=parseFloat(d.gocRoll||0),p=parseFloat(d.gocPitch||0),y=parseFloat(d.gocYaw||0);
  function up(k,v,t){const el=id('pv-'+k);if(el)el.parentElement.classList.toggle('err',Math.abs(finite(v)-finite(t))>20);}
  up('rr',d.tocDoRoll,d.tgtRateRoll);
  up('ar',d.gocRoll,d.angleTarget[0]);
  up('y',d.gocYaw,0);
  up('alt',d.doCao,d.altTarget);
  if(d.optXY){
    if(id('pv-opt')) id('pv-opt').textContent=d.optXY[0].toFixed(1)+', '+d.optXY[1].toFixed(1);
    if(id('sp-opt')) id('sp-opt').textContent='0.0, 0.0';
    if(id('er-opt')) id('er-opt').textContent='-';
    wOptX.push(finite(d.optXY[0])); wOptX.shift();
    wOptY.push(finite(d.optXY[1])); wOptY.shift();
    drawOptChart('chart-opt', wOptX, wOptY, 50);
  }
  const yawDisplay=d.compassOk?finite(d.headingDeg):wrap360(y);
  id('sv-roll').textContent=r.toFixed(1);id('sv-pitch').textContent=p.toFixed(1);id('sv-yaw').textContent=yawDisplay.toFixed(1);
  wR.push(r);wR.shift();wP.push(p);wP.shift();
  drawChart('waveChart',[{data:wR,color:'#f38ba8'},{data:wP,color:'#a6e3a1'}],45);
  const tgt=d.pidTarget||[0,0];
  id('sv-batt').textContent=battV.toFixed(2)+'V';
  id('sv-battPct').textContent=battPct.toFixed(0)+'%';
  id('bar-batt').style.width=battPct+'%';
  const altRaw=Number.isFinite(parseFloat(d.altNewM))?d.altNewM:d.doCao;
  const alt=Math.max(0,parseFloat(altRaw||0));
  const altOk=!!d.altSensorOk;
  altScale=Math.max(1,altScale*0.96,alt*1.25,finite(d.altTarget)*1.25);
  id('sv-alt').textContent=altOk?alt.toFixed(2)+' m':'-- m';
  id('bar-alt').style.width=altOk?Math.min(100,(alt/Math.max(1,altScale))*100)+'%':'0%';
  id('alt-scale-label').textContent='Auto '+(altScale<10?altScale.toFixed(1):altScale.toFixed(0))+' m';
  const angleT=d.angleTarget||[0,0];
  setTuneLive('rr',finite(d.gx),finite(tgt[0]),180,'#94e2d5');
  setTuneLive('pr',finite(d.gy),finite(tgt[1]),180,'#f38ba8');
  setTuneLive('yr',finite(d.tocDoYaw),0,180,'#f9e2af');
  setTuneLive('ar',r,finite(angleT[0]),45,'#89b4fa');
  setTuneLive('ap',p,finite(angleT[1]),45,'#a6e3a1');
  setTuneLive('alt',alt,finite(d.altTarget),Math.max(1,altScale),'#cba6f7');
  drawFlowMap(d);
  updateCompassUI(d);
  updateGpsUI(d);
  const ch=d.CH||[1500,1500,1000,1500,1000,1000];
  for(let i=1;i<=6;i++){
    id('val-ch'+i).textContent=sbusLive?ch[i-1]:'--';
    id('bar-ch'+i).style.width=sbusLive?pct(ch[i-1])+'%':'0%';
  }
  const mot=d.esc||[1000,1000,1000,1000];
  const mnIds=['mn1','mn2','mn3','mn4'];
  for(let i=0;i<4;i++){id('mv'+(i+1)).textContent=mot[i];id('vm'+(i+1)).textContent=mot[i];id('bm'+(i+1)).style.width=pct(mot[i])+'%';setMotorColor(mnIds[i],mot[i]);}
  const throttle=Math.max(mot[0]||1000,mot[1]||1000,mot[2]||1000,mot[3]||1000);
  setDroneMotorTargets(mot);
  const visualYaw=d.compassOk?finite(d.headingDeg):y;
  updateDroneVisual(r,p,visualYaw,throttle,!!d.daKichHoat);
  updatePidParamInputs(d);
  if(!pidLoaded){
    if(d.rOff!==undefined)id('off-roll').value=parseFloat(d.rOff).toFixed(1);
    if(d.pOff!==undefined)id('off-pitch').value=parseFloat(d.pOff).toFixed(1);
    pidLoaded=true;
  }
}
let fetchErrors=0, statusPollTimer=null, statusBusy=false;
function scheduleStatusFetch(delay){
  clearTimeout(statusPollTimer);
  const wait = delay !== undefined ? delay : STATUS_INTERVAL_MS;
  statusPollTimer = setTimeout(fetchStatus, wait);
}
async function fetchStatus(){
  if(statusBusy){scheduleStatusFetch(20);return;}
  statusBusy=true;
  try{
    const r=await fetch('/status?_='+Date.now(),{cache:'no-store'});
    if(!r.ok)throw new Error();
    updateUI(await r.json());fetchErrors=0;
  }catch(e){
    if(++fetchErrors>=5){
      id('b-sbus').className='badge badge-fail';
      id('b-sbus').lastElementChild.textContent='SBUS OFF';
      id('b-gps').className='badge badge-fail';
      id('b-gps').lastElementChild.textContent='GPS OFF';
      id('b-compass').className='badge badge-fail';
      id('b-compass').lastElementChild.textContent='COMPASS OFF';
      id('b-alt').className='badge badge-fail';
      id('b-alt').lastElementChild.textContent='ALT OFF';
      setSignal('sig-sbus','bars-sbus','sig-sbus-val',0,false);
    }
  }finally{
    statusBusy=false;
    scheduleStatusFetch();
  }
}
requestAnimationFrame(animateDroneVisual);
scheduleStatusFetch(0);
</script>
</body>
</html>
)HTML";
