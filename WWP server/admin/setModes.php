<?php
	require "CheckUser.php";
	require "errMsg.php";
	
	function setChannelModes($uid="")
	{
		global $con;
		$modes = "<SCHEME=";
		$channel = $_POST['Channel'];
		$channel = $con->real_escape_string($channel);
		if(empty($channel))
			return 0;
		$code = $_POST['modes'];
		if(empty($code))
			return 0;
		$i = 0;
		foreach($code as $key => $mode)
		{
			switch($mode)
			{
				case 'B':
				$bloodLvl = $_POST['bloodAmount'];
				if($bloodLvl > 'e')
					$bloodLvl = 'e';
				if($bloodLvl < 'a')
					$bloodLvl = 'a';	
				if($key == 0)
					$modes .= $mode . $bloodLvl;
				else
					$modes .= "," . $mode . $bloodLvl;
				break;
				case 'P':
				$ropePowerLvl = $_POST['ropePowerLvl'];	
				if($ropePowerLvl > 'k')
					$ropePowerLvl = 'k';
				if($ropePowerLvl < 'a')
					$ropePowerLvl = 'a';
				if($key == 0)
					$modes .= $mode . $ropePowerLvl;
				else	
					$modes .= ",".$mode . $ropePowerLvl;
				break;
				case 'T':
				$RankType = $_POST['RankType'];	
				if($key == 0)
					$modes .= $mode . $RankType;
				else
					$modes .= "," . $mode . $RankType;	
				break;
				case 'R':					
				$minRestriction = $_POST['rankMinRestriction'];
				$maxRestriction = $_POST['rankMaxRestriction'];						
				if($i == 0)
				{					
					if($key == 0)
						$modes .= $mode . $minRestriction;
					else	
						$modes .= "," . $mode.$minRestriction;
				}
				else
				{
					if($minRestriction > $maxRestriction)
					{
						$minRestriction ^= $maxRestriction;
						$maxRestriction ^= $minRestriction;
						$minRestriction ^= $maxRestriction;		
					}							
					if($maxRestriction <'a')
						$maxRestriction = 'a';
					if($maxRestriction > 'm')
						$maxRestriction = 'm';
					$modes .= $maxRestriction;
				}
				$i++;
				break;
				case 'W':				
				$wormsAmount = $_POST['wormsAmount'];
				if($key == 0)
					$modes .= $mode . $wormsAmount;
				else
					$modes .= "," . $mode . $wormsAmount;
				break;
				case 'D':					
					$turnTime = array("\x0f" => "\x00", "\x14" => "\x01", "\x1e" => "\x02", "\x2d" => "\x03", "\x3c" => "\x04", "\x5a" => "\x05");
					$fileName = $_FILES['forceScheme']['tmp_name'];
					if (!$fileName)
					{
						if($key == 0)
							$modes .= $mode . $_POST['forceScheme1'] . ",N".$channel;
						else
							$modes .= "," . $mode . $_POST['forceScheme1'] . ",N".$channel;							
						break;
					}
					$fileSize = $_FILES['forceScheme']['size'];
					$scheme = file_get_contents($fileName);
					$signature = substr($scheme,0,4);
					if(strcmp($signature,"SCHM"))
					{
						unlink($fileName);
						return 0;
					}
					else
					{
						$scheme[0x11] = chr(ord($scheme[0x11]) / 10);
						$scheme[0x13] = chr(ord($scheme[0x13]) / 10);
						$scheme[0x14] = chr(ord($scheme[0x14]) / 25);
						$scheme[0x15] = chr(ord($scheme[0x15]) / 10);
						$scheme[0x1a] = chr(ord($scheme[0x1a]) / 50);
						$scheme[0x1b] = chr(ord($turnTime[$scheme[0x1b]]));
						$scheme[0x1c] = chr(ord($scheme[0x1c]) / 5);
						$fileSize -= 8;

						for($i = 5;$i < $fileSize;$i++)
						{
							$networkScheme[$i] = chr(ord($scheme[$i]) + 0x61);
						}
					if($key == 0)
						$modes .= $mode . implode($networkScheme) . ",N".$channel;
					else
						$modes .= "," . $mode . implode($networkScheme) . ",N".$channel;						
						unlink($fileName);
					}
				break;	
				case 'G':
					$specialWeapons = $_POST['specialWeapons'];
					if($key == 0)
						$modes .= $mode . $specialWeapons;
					else
						$modes .= "," . $mode . $specialWeapons;				
				break;
			}
		}

		$modes .= ">\n";
		if($uid == "")
		{
			$command = "INSERT INTO `Schemes` (Channel, modes)
									VALUES('$channel', '$modes')";
		}
		else
			$command = "UPDATE `Schemes` SET Channel='$channel', modes='$modes' WHERE id='$uid'";
		$con->query($command) or die($con->error);

		return 1;
	}
	
	function viewChannelList()
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
				<div class="modesItem">
				<div class="header">
				List of worms Channels modes
				</div>	
				<div class="modesInfoDialog">
				<br/>
				<?php
				global $con;
				$command = "SELECT * FROM `Schemes`";
				$res = $con->query($command) or die($con->error);
				$schemeCnt = $res->num_rows;
				if($schemeCnt > 0)
				{
				?>
				<div style="margin:1%;float:left;width:12%;text-align:center;" onmouseover="document.getElementById('a').style.opacity=1.0;document.getElementById('a').filters.alpha.opacity=100" onmouseout="document.getElementById('a').style.opacity=0.4;document.getElementById('a').filters.alpha.opacity=40">
					<div>
					<a href="index.php?page=main">
					<img id="a" src="images/home.png" title="main" alt="main" />
					</a>
					</div>
					<div>
					<a href="index.php?page=main">main</a>
					</div>
				</div>					
				<div style="margin:1%;float:left;width:12%;text-align:center;" onmouseover="document.getElementById('b').style.opacity=1.0;document.getElementById('b').filters.alpha.opacity=100" onmouseout="document.getElementById('b').style.opacity=0.4;document.getElementById('b').filters.alpha.opacity=40">
					<div>
					<a href="index.php?page=setModes&amp;action=add">
					<img id="b" src="images/add.png" title="add channel" alt="add channel" />
					</a>
					</div>
					<div>
					<a href="index.php?page=setModes&amp;action=add" >add channel</a>
					</div>
				</div>		
				
				<br/>
				<table style="clear:both;">
					<tr>
					<th>ID</th>
					<th>Channel</th>
					<th>Modes</th>
					<th>Edit</th>
					<th>Delete</th>
					</tr>
					<?php
					$searchList = array("<", ">");
					while($schemeData = $res->fetch_assoc())
					{
						echo "<tr>\n<td>".$schemeData['id']."</td>\n";
						echo "<td>".$schemeData['Channel']."</td>\n";
						echo "<td>".htmlentities("<").rtrim(wordwrap(str_replace($searchList, "", $schemeData['modes']),40,"<br/>", true),"<br/>\n").htmlentities(">")."</td>\n";
						echo "<td> <a href=\"index.php?page=setModes&amp;action=edit&amp;uid=".$schemeData['id']."\"><img src='images/edit.png' title='edit' alt='edit' /></a></td>\n";
						echo "<td> <a href=\"index.php?page=setModes&amp;action=delete&amp;uid=".$schemeData['id']."\"><img src='images/delete.png' title='delete' alt='delete'/></a> </td>\n</tr>\n";
					}
					?>
				</table>
				<?php			
				}
				else
					header("location:index.php?page=setModes&action=add");
				?>
				</div>
			</div>		
		</body>
		</html>		
	<?php
	}
	
	function addChannel()
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
				Set Channel modes
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
				<div style="margin:1%;float:left;width:12%;text-align:center;" onmouseover="document.getElementById('b').style.opacity=1.0;document.getElementById('b').filters.alpha.opacity=100" onmouseout="document.getElementById('b').style.opacity=0.4;document.getElementById('b').filters.alpha.opacity=40">
					<div>
					<a href="index.php?page=setModes&amp;action=view">
					<img id="b" src="images/view.png" title="view" alt="view" style="opacity:0.4;"/>
					</a>
					</div>
					<div>
					<a href="index.php?page=setModes&amp;action=view" >view</a>
					</div>
				</div>							
					<form method="post" action="" enctype="multipart/form-data">
					
					<div style="float:left;clear:both;">
					<label>
					Channel name:
					</label>
					<input type="text" name="Channel" />
					</div>
					
					<div style="float:left;clear:both;">
					<label>
					Set channel modes:
					</label>
					<br/>
					<br/>
					</div>					
					<div>
					<div style="float:left;clear:both;">
					<input type="checkbox" name="modes[]" value="B"/>
					<label>Amount of blood</label>
					</div>
					<div style="float:right;">
					<select name="bloodAmount">
						<?php
						for($i = 'a' ;$i<='e'; $i++)
							echo "<option value='".$i."'>".$i."</option>";
						?>
					</select>
					</div>
					</div>
					
					<div>
					<div style="float:left;clear:both;">
					<input style="margin-bottom:15px;" type="checkbox" name="modes[]" value="P"/>
					<label>Rope pushing power level</label>
					</div>
					<div style="float:right;">
					<select name="ropePowerLvl">
						<?php
						for($i = 'a' ;$i<='k'; $i++)
							echo "<option value='".$i."'>".$i."</option>";
						?>

					</select>
					</div>
					</div>
					
					<div>
					<div style="float:left;clear:both;">
					<input style="margin-bottom:15px;" type="checkbox" name="modes[]" value="T"/>
					<label>Set ranked channel</label>
					</div>
					<div style="float:right;">	
					<select name="RankType">
						<?php
						for($i = 'a' ;$i<='f'; $i++)
							echo "<option value='".$i."'>".$i."</option>";
						?>

					</select>	
					</div>
					</div>		
					
					<div>
						<div style="float:left;clear:both;">
						<input style="margin-bottom:15px;" type="checkbox" name="modes[]" value="R"/>
						<label>Set minimun rank restriction</label>	
						
						</div>
						<div style="float:right;">
						<select name="rankMinRestriction">
							<?php
							for($i = 'a' ;$i<='m'; $i++)
								echo "<option value='".$i."'>".$i."</option>";
							?>

						</select>	
						</div>
					</div>
					
					<div>
						<div style="float:left;clear:both;">
						<input  type="checkbox" name="modes[]" value="R"/>
						<label">Set maximum rank restriction</label>	
						</div>
						<div style="float:right;">
						<select name="rankMaxRestriction">
							<?php
							for($i = 'a' ;$i<='m'; $i++)
								echo "<option value='".$i."'>".$i."</option>";
							?>

						</select>	
						</div>
					</div>	
					
					<div>
						<div style="float:left;clear:both;">
						<input style="margin-bottom:15px;" type="checkbox" name="modes[]" value="W"/>
						<label>Worms per team</label>
						</div>
						<div style="float:right;">
						<select name="wormsAmount">
							<?php
							for($i = 'b' ;$i<='i'; $i++)
								echo "<option value='".$i."'>".$i."</option>";
							?>	
						</select>
						</div>
					</div>
					
					<div>
						<div style="float:left;clear:both;">
						<input style="margin-bottom:15px;" type="checkbox" name="modes[]" value="D" />
						<label>Force Scheme</label>		
						</div>	
						<div style="float:right;">
						<input type="file" name="forceScheme" />	
						</div>
					</div>	
					<div>
						<div style="float:left;clear:both;">
						<input style="margin-bottom:15px;" type="checkbox" name="modes[]" value="G"/>
						<label>Special weapons</label>
						</div>
						<div style="float:right;">
						<textarea cols="40" rows="10" name="specialWeapons"></textarea>
						</div>
					</div>						
						<div>
						<div style="float:left;clear:both;">
						<input type="hidden" name="check" value="1" />
						<input type="submit" value="Set channel modes"/>	
						</div>
					</div>				
					</form>
				</div>
				</div>
				<div class="modesItem">
				<div class="header">
				Channel modes information
				</div>	
				<div class="modesInfoDialog">
				<b>AnythingGoes</b><br/>
				Pf,Be<br/>
				<b>PartyTime</b><br/>
				Ba<br/>
				<b>RopersHeaven</b><br/>
				Pf<br/>
				<br/>
				<b>a-z</b> is 0 to 25 unless otherwise stated.
				<br/>
				<table>
				<tr>
					<td><b>Code</b></td>
					<td><b>Arguments</b></td>
					<td><b>Effect</b></td>
				</tr>
				<tr>
					<td>B</td>
					<td>a-e</td>
					<td>Amount of blood.</td>
				</tr>
				<tr>
					<td>D</td>
					<td>a-z × ...</td>
					<td>String of enforced game/weapon options.</td>
				</tr>	
		<!--	not implemented	
				<tr>
					<td>E</td>
					<td>(None)</td>
					<td>Make the channel open (the default setting). Same as "Ta".</td>
				</tr>	-->	
				<tr>
					<td>G</td>
					<td>a-z × ...</td>
					<td>String of super-weapon options.</td>
				</tr>	
		<!--	not implemented			
				<tr>
					<td>L</td>
					<td>(text)</td>
					<td>Name of the built-in level required.</td>
				</tr>	-->	
				<tr>
					<td>N</td>
					<td>(text)</td>
					<td>Set the scheme name that will appear.</td>
				</tr>		
				<tr>
					<td>P</td>
					<td>a-k</td>
					<td>Set rope pushing power level, defaults to 0, f is normal.</td>
				</tr>		
				<tr>
					<td>R</td>
					<td>a-m<br/>a-m</td>
					<td>Set rank restriction (minimal and maximal rank)</td>
				</tr>	
		<!--	not implemented			
				<tr>
					<td>S</td>
					<td>a-...</td>
					<td>Set an enforced stock scheme.</td>
				</tr>	-->	
				<tr>
					<td>T</td>
					<td>a-f</td>
					<td>Hosting: 0=allowed, 1=ranked, 2=ranked, 3=ranked, 4=ranked, 5=disallowed<br/>
						This is also used as the type value of the game when reporting the result.</td>
				</tr>
		<!--	not implemented			
				<tr>
					<td>V</td>
					<td>a-...</td>
					<td>Restrict room to players of a certain type - internally stored as "clan number", but not many clans can fit in one byte...</td>
				</tr> -->	
				<tr>
					<td>W</td>
					<td>b-i</td>
					<td>Worms per team, counting from b=1. If adding a team with this many would exceed the worm limit, that team will be added with less worms. Any values out of range count as b (1).</td>
				</tr>			
				</table>
					
				</div>
			</div>
		</body>
		</html>
		<?php
	}

function editChannel($uid)
{
	global $con;
	$command = "SELECT * FROM `Schemes` WHERE id='$uid'";
	$res = $con->query($command) or die($con->error);
	$isSchemeExist = $res->num_rows;
	if($isSchemeExist == 0)	
		return 0;
		
	$schemeData = $res->fetch_assoc();
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
			Set Channel modes
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
				<div style="margin:1%;float:left;width:12%;text-align:center;" onmouseover="document.getElementById('b').style.opacity=1.0;document.getElementById('b').filters.alpha.opacity=100" onmouseout="document.getElementById('b').style.opacity=0.4;document.getElementById('b').filters.alpha.opacity=40">
					<div>
					<a href="index.php?page=setModes&amp;action=view">
					<img id="b" src="images/view.png" title="view" alt="view" style="opacity:0.4;"/>
					</a>
					</div>
					<div>
					<a href="index.php?page=setModes&amp;action=view" >view</a>
					</div>
				</div>		
				<form method="post" action="" enctype="multipart/form-data">
				
				<div style="float:left;clear:both;">
				<label>
				Channel name:
				</label>
				<input type="text" name="Channel" value="<?php echo $schemeData['Channel'];?>"/>
				</div>
				<div style="float:left;clear:both;">
				<label>
				Set channel modes:
				</label>
				<br/>
				<br/>
				</div>					
				<div>
				<div style="float:left;clear:both;">
				<?php
					$modes = htmlspecialchars($schemeData['modes']);
					$pos = strpos($modes,'B');
					if($pos == false)
					{
						echo "<input type=\"checkbox\" name=\"modes[]\" value=\"B\"/>";
						$val = "";
					}
					else
					{
						echo "<input type=\"checkbox\" name=\"modes[]\" value=\"B\" checked />";
						$val = substr($modes,$pos+1,1);					
					}
				?>
				<label>Amount of blood</label>			
				</div>
				<div style="float:right;">
				<select name="bloodAmount">
					<?php
					for($i = 'a' ;$i<='e'; $i++)
					{
						if($i == $val)
							echo "<option value='".$i."' selected>".$i."</option>";
						else	
							echo "<option value='".$i."'>".$i."</option>";							
					}
					?>
				</select>
				</div>
				</div>
				
				<div>
				<div style="float:left;clear:both;">
				<?php
				$pos = strpos($modes,'P');
				if($pos == false)
				{
					echo "<input type=\"checkbox\" name=\"modes[]\" value=\"P\"/>";
					$val = "";
				}
				else
				{
					echo "<input type=\"checkbox\" name=\"modes[]\" value=\"P\" checked />";
					$val = substr($modes,$pos+1,1);						
				}
				?>
				<label>Rope pushing power level</label>
				</div>
				<div style="float:right;">
				<select name="ropePowerLvl">
					<?php
					for($i = 'a' ;$i<='k'; $i++)
					{
						if($i == $val)
							echo "<option value='".$i."' selected>".$i."</option>";
						else	
							echo "<option value='".$i."'>".$i."</option>";							
					}
					?>

				</select>
				</div>
				</div>
				
				<div>
				<div style="float:left;clear:both;">
				<?php
				$pos = strpos($modes,'T');
				if($pos == false)
				{
					echo "<input type=\"checkbox\" name=\"modes[]\" value=\"T\"/>";
					$val = "";
				}	
				else
				{
					echo "<input type=\"checkbox\" name=\"modes[]\" value=\"T\" checked />";
					$val = substr($modes,$pos+1,1);					
				}
				?>
				<label>Set ranked channel</label>
				</div>
				<div style="float:right;">	
				<select name="RankType">
					<?php
					for($i = 'a' ;$i<='f'; $i++)
					{
						if($i == $val)
							echo "<option value='".$i."' selected>".$i."</option>";
						else	
							echo "<option value='".$i."'>".$i."</option>";							
					}
					?>

				</select>	
				</div>
				</div>		
				
				<div>
					<div style="float:left;clear:both;">
					<?php
					$pos = strpos($modes,'R');
					if($pos == false)
					{
						echo "<input type=\"checkbox\" name=\"modes[]\" value=\"R\"/>";
						$val = "";
					}	
					else
					{
						echo "<input type=\"checkbox\" name=\"modes[]\" value=\"R\" checked />";
						$val = substr($modes,$pos+1,1);						
					}
					?>
					<label>Set minimun rank restriction</label>						
					</div>
					<div style="float:right;">
					<select name="rankMinRestriction">
						<?php
						for($i = 'a' ;$i<='m'; $i++)
						{
							if($i == $val)
								echo "<option value='".$i."' selected>".$i."</option>";
							else	
								echo "<option value='".$i."'>".$i."</option>";							
						}
						?>

					</select>	
					</div>
				</div>
				
				<div>
					<div style="float:left;clear:both;">
					<?php
					$pos = strpos($modes,'R');
					$checkMode = substr($modes, $pos+1, 2);
					if($pos == false || ctype_lower($checkMode) == "")
					{
						echo "<input type=\"checkbox\" name=\"modes[]\" value=\"R\"/>";
						$val = "";
					}
					else
					{
						echo "<input type=\"checkbox\" name=\"modes[]\" value=\"R\" checked />";
						$val = substr($modes,$pos+2,1);					
					}
					?>
					<label">Set maximum rank restriction</label>	
					</div>
					<div style="float:right;">
					<select name="rankMaxRestriction">
						<?php
						for($i = 'a' ;$i<='m'; $i++)
						{
							if($i == $val)
								echo "<option value='".$i."' selected>".$i."</option>";
							else	
								echo "<option value='".$i."'>".$i."</option>";							
						}	
						?>

					</select>	
					</div>
				</div>	
				
				<div>
					<div style="float:left;clear:both;">
					<?php
					$pos = strpos($modes,'W');
					if($pos == false)
						echo "<input type=\"checkbox\" name=\"modes[]\" value=\"W\"/>";
					else
					{
						echo "<input type=\"checkbox\" name=\"modes[]\" value=\"W\" checked />";
						$val = substr($modes,$pos+1,1);					
					}
					?>
					<label>Worms per team</label>
					</div>
					<div style="float:right;">
					<select name="wormsAmount">
						<?php
						for($i = 'b' ;$i<='i'; $i++)
						{
							if($i == $val)
								echo "<option value='".$i."' selected>".$i."</option>";
							else	
								echo "<option value='".$i."'>".$i."</option>";							
						}
						?>	
					</select>
					</div>
				</div>
				
				<div>
					<div style="float:left;clear:both;">
					<?php
					$pos = strpos($modes,'D');	
					$pos1 = strpos($modes,",",$pos+1);
					if($pos1 == false)
						$pos1 = strpos($modes,htmlspecialchars(">"),$pos+1);
					$len = $pos1 - $pos - 1;
					if($pos == false)
						echo "<input type=\"checkbox\" name=\"modes[]\" value=\"D\"/>";
					else
					{
						echo "<input type=\"checkbox\" name=\"modes[]\" value=\"D\" checked />";						
						$val = substr($modes,$pos+1,$len);
						
					}
					?>
					<label>Force Scheme</label>		
					</div>
					<div style="float:right;">
					<input type="file" name="forceScheme" />
					<?php
					if($pos != false)
						echo "<input type='hidden' name='forceScheme1' value='".$val."' />";
					?>
					</div>					
				</div>	
				<div>
					<div style="float:left;clear:both;">
					<?php
					$pos = strrpos($modes,'G');	
					$pos1 = strpos($modes,",",$pos+1);
					if($pos1 == false)
						$pos1 = strpos($modes,htmlspecialchars(">"),$pos+1);
					$len = $pos1 - $pos - 1;	
					if($pos == false)
					{					
						echo "<input type=\"checkbox\" name=\"modes[]\" value=\"G\"/>";
						echo "<label>Special weapons</label>\n</div>\n
							  <div style='float:right;'>\n
							  <textarea cols='40' rows='10' name='specialWeapons'></textarea>\n
							  </div>\n";
					}
					else
					{
						echo "<input type=\"checkbox\" name=\"modes[]\" value=\"G\" checked />";						
						$val = substr($modes,$pos+1,$len);
						echo "<label>Special weapons</label>\n
							</div>\n
							<div style='float:right;'>\n
							<textarea cols='40' rows='10' name='specialWeapons'>".$val."</textarea>\n
							</div>\n";
					}					
					?>

				</div>					

			
					<div>
					<div style="float:left;clear:both;">
					<input type="hidden" name="check" value="1" />
					<input type="submit" value="Set channel modes"/>	
					</div>
				</div>				
				</form>
			</div>
			</div>
			<div class="modesItem">
			<div class="header">
			Channel modes information
			</div>	
			<div class="modesInfoDialog">
			<b>AnythingGoes</b><br/>
			Pf,Be<br/>
			<b>PartyTime</b><br/>
			Ba<br/>
			<b>RopersHeaven</b><br/>
			Pf<br/>
			<br/>
			<b>a-z</b> is 0 to 25 unless otherwise stated.
			<br/>
			<table>
			<tr>
				<td><b>Code</b></td>
				<td><b>Arguments</b></td>
				<td><b>Effect</b></td>
			</tr>
			<tr>
				<td>B</td>
				<td>a-e</td>
				<td>Amount of blood.</td>
			</tr>
			<tr>
				<td>D</td>
				<td>a-z × ...</td>
				<td>String of enforced game/weapon options.</td>
			</tr>	
	<!--	not implemented	
			<tr>
				<td>E</td>
				<td>(None)</td>
				<td>Make the channel open (the default setting). Same as "Ta".</td>
			</tr>	-->	
			<tr>
				<td>G</td>
				<td>a-z × ...</td>
				<td>String of super-weapon options.</td>
			</tr>	
	<!--	not implemented			
			<tr>
				<td>L</td>
				<td>(text)</td>
				<td>Name of the built-in level required.</td>
			</tr>	-->	
			<tr>
				<td>N</td>
				<td>(text)</td>
				<td>Set the scheme name that will appear.</td>
			</tr>		
			<tr>
				<td>P</td>
				<td>a-k</td>
				<td>Set rope pushing power level, defaults to 0, f is normal.</td>
			</tr>		
			<tr>
				<td>R</td>
				<td>a-m<br/>a-m</td>
				<td>Set rank restriction (minimal and maximal rank)</td>
			</tr>	
	<!--	not implemented			
			<tr>
				<td>S</td>
				<td>a-...</td>
				<td>Set an enforced stock scheme.</td>
			</tr>	-->	
			<tr>
				<td>T</td>
				<td>a-f</td>
				<td>Hosting: 0=allowed, 1=ranked, 2=ranked, 3=ranked, 4=ranked, 5=disallowed<br/>
					This is also used as the type value of the game when reporting the result.</td>
			</tr>
	<!--	not implemented			
			<tr>
				<td>V</td>
				<td>a-...</td>
				<td>Restrict room to players of a certain type - internally stored as "clan number", but not many clans can fit in one byte...</td>
			</tr> -->	
			<tr>
				<td>W</td>
				<td>b-i</td>
				<td>Worms per team, counting from b=1. If adding a team with this many would exceed the worm limit, that team will be added with less worms. Any values out of range count as b (1).</td>
			</tr>			
			</table>
				
			</div>
		</div>
	</body>
	</html>	
	<?php
}

function deleteChannel($uid)
{
	global $con;
	$command ="DELETE FROM `Schemes` WHERE id='$uid'";
	$con->query($command) or die(setErrorMsg("cant delete channel<br/>", $con->error, "index.php?page=main" , "index.php?page=setModes&amp;action=view", "try delete other channel"));
	if($con->affected_rows)
		return 1;
	else
		return 0;
	
}

if($isAdmin == 1)
{				
	if(isset($_POST['check']))
	{	
		switch($_GET['action'])
		{
			case "add":
				$ok = setChannelModes();
				if($ok == 1)
					setSuccessMsg("channel modes has been set<br/>", "succeed set modes", "index.php?page=setModes&amp;action=add", "index.php?page=main", "add more channel?");
				else
					setErrorMsg("failed to set channel modes<br/>", "failed set modes", "index.php?page=setModes&amp;action=add", "index.php?page=main", "try to add new channel again?");
				break;
			case "edit":			
				$ok = setChannelModes($_GET['uid']);
				if($ok == 1)
					setSuccessMsg("channel modes has been set<br/>", "succeed edit modes", "index.php?page=setModes&amp;action=view", "index.php?page=main", "edit more channel?");
				else
					setErrorMsg("failed to set channel modes<br/>", "failed edit modes", "index.php?page=setModes", "index.php?page=main", "try to edit other channel?");
			break;
		}
	}
	else
	{
		switch($_GET['action'])
		{
			case "view":
				viewChannelList();	
				break;
			case "add":
				addChannel();
				break;
			case "edit":
				editChannel($_GET['uid']);
				break;
			case "delete":
				$ok = deleteChannel($_GET['uid']);
				if($ok == 1)
					setSuccessMsg("channel modes has been removed<br/>", "succeed remove channel", "index.php?page=setModes&amp;action=view", "index.php?page=main", "delete more channel?");
				else
					setErrorMsg("failed to remove channel <br/>", "failed delete channel", "index.php?page=setModes", "index.php?page=main", "try to delete other channel?");
				break;
			default:
			addChannel();
		}
			
			
	}
}
else
{
	header("location: login.php");
	die();
}
?>