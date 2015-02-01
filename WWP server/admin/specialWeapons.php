<?php
	require "CheckUser.php";
	require "errMsg.php";
	
	
	
	function setSpecialWeapons()
	{
		?>
		<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
			"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
		<html xmlns="http://www.w3.org/1999/xhtml">
		<head>
			<meta http-equiv="Content-type" content="text/html; charset=UTF-8" />
			<link rel="stylesheet" type="text/css" href="style.css" />
			<title>Admin Panel</title>
		</head>
		<body>
			<?php require "header.php"; ?>
			<div class="modesWindow">
			<div class="modesItem">
				<div class="header">
				Set Special weapons
				</div>	
				<div class="dialog">
				<div style="margin:1%;float:left;width:12%;text-align:center;" onmouseover="document.getElementById('a').style.opacity=1.0;document.getElementById('a').filters.alpha.opacity=100" onmouseout="document.getElementById('a').style.opacity=0.4;document.getElementById('a').filters.alpha.opacity=40">
					<div>
					<a href="index.php?page=main">
					<img id="a" src="images/home.png" title="main" alt="main" style="opacity:0.4;"/>
					</a>
					</div>
					<div>
					<a href="index.php?page=main">main</a>
					</div>
				</div>		
					<form method="post" action="">		
					<div>	
						<div>
							<div style="width:35%;float:left;clear:both;">
							<input type="checkbox" name="modes[]" value="ha"/>
							<label>Crate shower turns</label>
							</div>
							<div style="padding-left:4%;float:left;">
							<select name="crateShowerTurn">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>
						
						<div>
							<div style="width:35%;float:left;clear:both;">
							<label>Crate shower</label>
							</div>					
							<div style="padding-left:4%;float:left;"> 
							<select name="crateShower">
								<?php
								for($i = 'a' ;$i<='b'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>
					</div>
					<div>
						<div>
							<div style="padding-top:5%;width:35%;float:left;clear:both;">
							<input type="checkbox" name="modes[]" value="fs"/>
							<label>Crate spy turns</label>
							</div>
							<div style="padding-top:5%;padding-left:4%;float:left;">
							<select name="crateSpy">
								<?php
								for($i = 'a' ;$i<='b'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>					
						<div>
							<div style="width:35%;float:left;clear:both;">
							<label>crate spy turns</label>
							</div>					
							<div style="padding-left:4%;float:left;"> 
							<select name="spyTurnsNumber">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>	
					</div>
					<div>
						<div>
							<div style="padding-top:5%;width:35%;float:left;clear:both;">
							<input type="checkbox" name="modes[]" value="fq"/>
							<label>Invisibility</label>
							</div>
							<div style="padding-top:5%;padding-left:4%;float:left;">
							<select name="invisibility">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>					
						<div>
							<div style="width:35%;float:left;clear:both;">
							<label>Ammo delay</label>
							</div>					
							<div style="padding-left:4%;float:left;"> 
							<select name="invisibilityDelay">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>	
					</div>
					<div>
						<div>
							<div style="padding-top:5%;width:35%;float:left;clear:both;">
							<input type="checkbox" name="modes[]" value="eq"/>
							<label>Armageddon</label>
							</div>
							<div style="padding-top:5%;padding-left:4%;float:left;">
							<select name="armageddon">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>					
						<div>
							<div style="width:35%;float:left;clear:both;">
							<label>Ammo delay</label>
							</div>					
							<div style="padding-left:4%;float:left;"> 
							<select name="armageddonDelay">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>	
					</div>	
					<div>
						<div>
							<div style="padding-top:5%;width:35%;float:left;clear:both;">
							<input type="checkbox" name="modes[]" value="cq"/>
							<label>Girder pack</label>
							</div>
							<div style="padding-top:5%;padding-left:4%;float:left;">
							<select name="girderPack">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>					
						<div>
							<div style="width:35%;float:left;clear:both;">
							<label>Ammo delay</label>
							</div>					
							<div style="padding-left:4%;float:left;"> 
							<select name="girderPackDelay">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>	
					</div>	
					<div>
						<div>
							<div style="padding-top:5%;width:35%;float:left;clear:both;">
							<input type="checkbox" name="modes[]" value="fb"/>
							<label>Carpet bomb</label>
							</div>
							<div style="padding-top:5%;padding-left:4%;float:left;">
							<select name="carpetBomb">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>					
						<div>
							<div style="width:35%;float:left;clear:both;">
							<label>Ammo delay</label>
							</div>					
							<div style="padding-left:4%;float:left;"> 
							<select name="carpetBombDelay">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>	
					</div>	
					<div>
						<div>
							<div style="padding-top:5%;width:35%;float:left;clear:both;">
							<input type="checkbox" name="modes[]" value="dy"/>
							<label>Donkey</label>
							</div>
							<div style="padding-top:5%;padding-left:4%;float:left;">
							<select name="donkey">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>					
						<div>
							<div style="width:35%;float:left;clear:both;">
							<label>Ammo delay</label>
							</div>					
							<div style="padding-left:4%;float:left;"> 
							<select name="donkeyDelay">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>	
					</div>		
					<div>
						<div>
							<div style="padding-top:5%;width:35%;float:left;clear:both;">
							<input type="checkbox" name="modes[]" value="ba"/>
							<label>Earth quake</label>
							</div>
							<div style="padding-top:5%;padding-left:4%;float:left;">
							<select name="earthQuake">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>					
						<div>
							<div style="width:35%;float:left;clear:both;">
							<label>Ammo delay</label>
							</div>					
							<div style="padding-left:4%;float:left;"> 
							<select name="earthQuakeDelay">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>	
					</div>	
					<div>
						<div>
							<div style="padding-top:5%;width:35%;float:left;clear:both;">
							<input type="checkbox" name="modes[]" value="ga"/>
							<label>Freeze</label>
							</div>
							<div style="padding-top:5%;padding-left:4%;float:left;">
							<select name="freeze">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>					
						<div>
							<div style="width:35%;float:left;clear:both;">
							<label>Ammo delay</label>
							</div>					
							<div style="padding-left:4%;float:left;"> 
							<select name="freezeDelay">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>	
					</div>	
					<div>
						<div>
							<div style="padding-top:5%;width:35%;float:left;clear:both;">
							<input type="checkbox" name="modes[]" value="gb"/>
							<label>Magic bullet</label>
							</div>
							<div style="padding-top:5%;padding-left:4%;float:left;">
							<select name="magicBullet">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>					
						<div>
							<div style="width:35%;float:left;clear:both;">
							<label>Ammo delay</label>
							</div>					
							<div style="padding-left:4%;float:left;"> 
							<select name="magicBulletDelay">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>	
					</div>		
					<div>
						<div>
							<div style="padding-top:5%;width:35%;float:left;clear:both;">
							<input type="checkbox" name="modes[]" value="dq"/>
							<label>Mb bomb</label>
							</div>
							<div style="padding-top:5%;padding-left:4%;float:left;">
							<select name="mbBomb">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>					
						<div>
							<div style="width:35%;float:left;clear:both;">
							<label>Ammo delay</label>
							</div>					
							<div style="padding-left:4%;float:left;"> 
							<select name="mbBombDelay">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>	
					</div>	
					<div>
						<div>
							<div style="padding-top:5%;width:35%;float:left;clear:both;">
							<input type="checkbox" name="modes[]" value="da"/>
							<label>Mine strike</label>
							</div>
							<div style="padding-top:5%;padding-left:4%;float:left;">
							<select name="mineStrike">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>					
						<div>
							<div style="width:35%;float:left;clear:both;">
							<label>Ammo delay</label>
							</div>					
							<div style="padding-left:4%;float:left;"> 
							<select name="mineStrikeDelay">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>	
					</div>	
					<div>
						<div>
							<div style="padding-top:5%;width:35%;float:left;clear:both;">
							<input type="checkbox" name="modes[]" value="dt"/>
							<label>Ming Vase</label>
							</div>
							<div style="padding-top:5%;padding-left:4%;float:left;">
							<select name="mingVase">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>					
						<div>
							<div style="width:35%;float:left;clear:both;">
							<label>Ammo delay</label>
							</div>					
							<div style="padding-left:4%;float:left;"> 
							<select name="mingVaseDelay">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>	
					</div>	
					<div>
						<div>
							<div style="padding-top:5%;width:35%;float:left;clear:both;">
							<input type="checkbox" name="modes[]" value="db"/>
							<label>Mole Squadron</label>
							</div>
							<div style="padding-top:5%;padding-left:4%;float:left;">
							<select name="moleSquadron">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>					
						<div>
							<div style="width:35%;float:left;clear:both;">
							<label>Ammo delay</label>
							</div>					
							<div style="padding-left:4%;float:left;"> 
							<select name="moleSquadronDelay">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>	
					</div>	
					<div>
						<div>
							<div style="padding-top:5%;width:35%;float:left;clear:both;">
							<input type="checkbox" name="modes[]" value="dz"/>
							<label>Nuclear Test</label>
							</div>
							<div style="padding-top:5%;padding-left:4%;float:left;">
							<select name="nuclerTest">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>					
						<div>
							<div style="width:35%;float:left;clear:both;">
							<label>Ammo delay</label>
							</div>					
							<div style="padding-left:4%;float:left;"> 
							<select name="nuclerTestDelay">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>	
					</div>		
					<div>
						<div>
							<div style="padding-top:5%;width:35%;float:left;clear:both;">
							<input type="checkbox" name="modes[]" value="bt"/>
							<label>Mail Strike</label>
							</div>
							<div style="padding-top:5%;padding-left:4%;float:left;">
							<select name="mailStrike">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>					
						<div>
							<div style="width:35%;float:left;clear:both;">
							<label>Ammo delay</label>
							</div>					
							<div style="padding-left:4%;float:left;"> 
							<select name="mailStrikeDelay">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>	
					</div>		
					<div>
						<div>
							<div style="padding-top:5%;width:35%;float:left;clear:both;">
							<input type="checkbox" name="modes[]" value="cz"/>
							<label>Salvation Army</label>
							</div>
							<div style="padding-top:5%;padding-left:4%;float:left;">
							<select name="salvationArmy">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>					
						<div>
							<div style="width:35%;float:left;clear:both;">
							<label>Ammo delay</label>
							</div>					
							<div style="padding-left:4%;float:left;"> 
							<select name="salvationArmyDelay">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>	
					</div>		
					<div>
						<div>
							<div style="padding-top:5%;width:35%;float:left;clear:both;">
							<input type="checkbox" name="modes[]" value="eb"/>
							<label>Scales Of Justice</label>
							</div>
							<div style="padding-top:5%;padding-left:4%;float:left;">
							<select name="scalesOfJustice">
								<?php
								for($i = 'b' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>					
						<div>
							<div style="width:35%;float:left;clear:both;">
							<label>Ammo delay</label>
							</div>					
							<div style="padding-left:4%;float:left;"> 
							<select name="scalesOfJusticeDelay">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>	
					</div>	
					<div>
						<div>
							<div style="padding-top:5%;width:35%;float:left;clear:both;">
							<input type="checkbox" name="modes[]" value="et"/>
							<label>select worm</label>
							</div>
							<div style="padding-top:5%;padding-left:4%;float:left;">
							<select name="selectWorm">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>					
						<div>
							<div style="width:35%;float:left;clear:both;">
							<label>Ammo delay</label>
							</div>					
							<div style="padding-left:4%;float:left;"> 
							<select name="selectWormDelay">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>	
					</div>		
					<div>
						<div>
							<div style="padding-top:5%;width:35%;float:left;clear:both;">
							<input type="checkbox" name="modes[]" value="fa"/>
							<label>sheep strike</label>
							</div>
							<div style="padding-top:5%;padding-left:4%;float:left;">
							<select name="sheepStrike">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>					
						<div>
							<div style="width:35%;float:left;clear:both;">
							<label>Ammo delay</label>
							</div>					
							<div style="padding-left:4%;float:left;"> 
							<select name="sheepStrikeDelay">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>	
					</div>		
					<div>
						<div>
							<div style="padding-top:5%;width:35%;float:left;clear:both;">
							<input type="checkbox" name="modes[]" value="bj"/>
							<label>Suicide Bomber</label>
							</div>
							<div style="padding-top:5%;padding-left:4%;float:left;">
							<select name="suicideBomber">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>					
						<div>
							<div style="width:35%;float:left;clear:both;">
							<label>Ammo delay</label>
							</div>					
							<div style="padding-left:4%;float:left;"> 
							<select name="suicideBomberDelay">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>	
					</div>	
					<div>
						<div>
							<div style="padding-top:5%;width:35%;float:left;clear:both;">
							<input type="checkbox" name="modes[]" value="ec"/>
							<label>Super banana bomb</label>
							</div>
							<div style="padding-top:5%;padding-left:4%;float:left;">
							<select name="sBananaBomb">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>					
						<div>
							<div style="width:35%;float:left;clear:both;">
							<label>Ammo delay</label>
							</div>					
							<div style="padding-left:4%;float:left;"> 
							<select name="sBananaBombDelay">
								<?php
								for($i = 'a' ;$i<='k'; $i++)
									echo "<option value='".$i."'>".$i."</option>";
								?>
							</select>
							</div>
						</div>	
					</div>						
						<div>
						<div style="padding-top:5%;float:left;clear:both;">
						<input type="hidden" name="check" value="1" />
						<input type="submit" value="Set Special weapons"/>	
						</div>
					</div>				
					</form>
				</div>
				</div>
				<div class="modesItem">
				<div class="header">
				Special weapons information
				</div>	
				<div class="modesInfoDialog">
				<br/>
				<b>a-z</b> is 0 to 25 unless otherwise stated.
				<br/>
				<b>k</b> in Amount for unlimit weapons<br/>
				<b>k</b> in delay for disable weapon unless otherwise stated<br/><br/>
				<table>
				<tr>
					<td><b>Code</b></td>
					<td><b>Amount</b></td>
					<td><b>Delay</b></td>
					<td><b>Effect</b></td>
				</tr>
				<tr>
					<td>ha</td>
					<td>a-k</td>
					<td>a-b</td>
					<td>
					Crate shower, for unlimit Crate shower turns it would be set to k.<br/>
					</td>
				</tr>
				<tr>
					<td>fs</td>
					<td>a-b</td>
					<td>a-k</td>
					<td>Crate spy, how much turns Crate spy would be set, k for always.</td>
				</tr>	
				<tr>
					<td>fq</td>
					<td>a-k</td>
					<td>a-k</td>
					<td>Invisibility.</td>
				</tr>	
				<tr>
					<td>eq</td>
					<td>a-k</td>
					<td>a-k</td>
					<td>Armageddon.</td>
				</tr>	
				<tr>
					<td>cq</td>
					<td>a-k</td>
					<td>a-k</td>
					<td>Girder pack.</td>
				</tr>	
				<tr>
					<td>fb</td>
					<td>a-k</td>
					<td>a-k</td>
					<td>Carpet bomb.</td>
				</tr>	
				<tr>
					<td>dy</td>
					<td>a-k</td>
					<td>a-k</td>
					<td>Donkey.</td>
				</tr>		
				<tr>
					<td>ba</td>
					<td>a-k</td>
					<td>a-k</td>
					<td>Earth quake.</td>
				</tr>	
				<tr>
					<td>ga</td>
					<td>a-k</td>
					<td>a-k</td>
					<td>Freeze.</td>
				</tr>	
				<tr>
					<td>gb</td>
					<td>a-k</td>
					<td>a-k</td>
					<td>Magic bullet.</td>
				</tr>		
				<tr>
					<td>dq</td>
					<td>a-k</td>
					<td>a-k</td>
					<td>MB bomb.</td>
				</tr>		
				<tr>
					<td>da</td>
					<td>a-k</td>
					<td>a-k</td>
					<td>Mine strike.</td>
				</tr>	
				<tr>
					<td>dt</td>
					<td>a-k</td>
					<td>a-k</td>
					<td>Ming vase.</td>
				</tr>	
				<tr>
					<td>db</td>
					<td>a-k</td>
					<td>a-k</td>
					<td>Mole Squadron.</td>
				</tr>	
				<tr>
					<td>dz</td>
					<td>a-k</td>
					<td>a-k</td>
					<td>Nuclear Test.</td>
				</tr>		
				<tr>
					<td>bt</td>
					<td>a-k</td>
					<td>a-k</td>
					<td>Mail strike.</td>
				</tr>	
				<tr>
					<td>cz</td>
					<td>a-k</td>
					<td>a-k</td>
					<td>Salvation Army.</td>
				</tr>	
				<tr>
					<td>eb</td>
					<td>b-k</td>
					<td>a-k</td>
					<td>Scales Of Justice.</td>
				</tr>	
				<tr>
					<td>et</td>
					<td>a-k</td>
					<td>a-k</td>
					<td>Select worm.</td>
				</tr>	
				<tr>
					<td>fa</td>
					<td>a-k</td>
					<td>a-k</td>
					<td>Sheep strike.</td>
				</tr>	
				<tr>
					<td>bj</td>
					<td>a-k</td>
					<td>a-k</td>
					<td>Suicide Bomber.</td>
				</tr>	
				<tr>
					<td>ec</td>
					<td>a-k</td>
					<td>a-k</td>
					<td>Super banana bomb.</td>
				</tr>				
				</table>
					
				</div>
			</div>				
		</body>
		</html>
		<?php
	}
	
	function setWeapons($weapons)
	{
		$code = $_POST['modes'];
		$modes = "";
		foreach($code as $mode)
		{
			switch($mode)
			{
				case "ha":
					$modes .= $mode.$_POST['crateShowerTurn'].$_POST['crateShower'];
				break;
				case "fs":
					$modes .= $mode.$_POST['crateSpy'].$_POST['spyTurnsNumber'];
				break;
				case "fq":
					$modes .= $mode.$_POST['invisibility'].$_POST['invisibilityDelay'];
				break;
				case "eq":
					$modes .= $mode.$_POST['armageddon'].$_POST['armageddonDelay'];
				break;
				case "cq":
					$modes .= $mode.$_POST['girderPack'].$_POST['girderPackDelay'];
				break;
				case "fb":
					$modes .= $mode.$_POST['carpetBomb'].$_POST['carpetBombDelay'];
				break;			
				case "dy":
					$modes .= $mode.$_POST['donkey'].$_POST['donkeyDelay'];			
				break;
				case "ba":
					$modes .= $mode.$_POST['earthQuake'].$_POST['earthQuakeDelay'];	
				break;
				case "ga":
					$modes .= $mode.$_POST['freeze'].$_POST['freezeDelay'];					
				break;
				case "gb":
					$modes .= $mode.$_POST['magicBullet'].$_POST['magicBulletDelay'];	
				break;
				case "dq":
					$modes .= $mode.$_POST['mbBomb'].$_POST['mbBombDelay'];	
				break;
				case "da":
					$modes .= $mode.$_POST['mineStrike'].$_POST['mineStrikeDelay'];
				break;
				case "dt":
					$modes .= $mode.$_POST['mingVase'].$_POST['mingVaseDelay'];
				break;
				case "db":
					$modes .= $mode.$_POST['moleSquadron'].$_POST['moleSquadronDelay'];
				break;
				case "dz":
					$modes .= $mode.$_POST['nuclerTest'].$_POST['nuclerTestDelay'];
				break;
				case "bt":
					$modes .= $mode.$_POST['mailStrike'].$_POST['mailStrikeDelay'];
				break;
				case "cz":
					$modes .= $mode.$_POST['salvationArmy'].$_POST['salvationArmyDelay'];
				break;
				case "eb":
					$modes .= $mode.$_POST['scalesOfJustice'].$_POST['scalesOfJustice'];
				break;
				case "et":
					$modes .= $mode.$_POST['selectWorm'].$_POST['selectWormDelay'];
				break;
				case "fa":
					$modes .= $mode.$_POST['sheepStrike'].$_POST['sheepStrikeDelay'];
				break;
				case "bj":
					$modes .= $mode.$_POST['suicideBomber'].$_POST['suicideBomberDelay'];
				break;
				case "ec":
					$modes .= $mode.$_POST['sBananaBomb'].$_POST['sBananaBombDelay'];
				break;		
			}
		}
		return $modes;
	}
	
if($isAdmin == 1)
{				
	if(isset($_POST['check']))
	{
		$specialWeapons = setWeapons($_POST);
		setSuccessMsg("copy the text for set special weapons on channel<br/><textarea cols='40' rows='10'>".$specialWeapons."</textarea><br/>", "special weapons has been converted", "index.php?page=setModes&amp;action=view", "index.php?page=main", "configure special weapons?<br/>");
	}
	else
	{
		setSpecialWeapons();					
	}
}
else
{
	header("location: login.php");
	die();
}
?>