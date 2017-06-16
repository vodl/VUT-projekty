<?php //netteCache[01]000388a:2:{s:4:"time";s:21:"0.85758900 1354987381";s:9:"callbacks";a:2:{i:0;a:3:{i:0;a:2:{i:0;s:19:"Nette\Caching\Cache";i:1;s:9:"checkFile";}i:1;s:66:"/home/users/xd/xdlapa01/WWW/IIS/app/templates/Prodej/default.latte";i:2;i:1354986341;}i:1;a:3:{i:0;a:2:{i:0;s:19:"Nette\Caching\Cache";i:1;s:10:"checkConst";}i:1;s:25:"Nette\Framework::REVISION";i:2;s:30:"6a33aa6 released on 2012-10-01";}}}?><?php

// source file: /home/users/xd/xdlapa01/WWW/IIS/app/templates/Prodej/default.latte

?><?php
// prolog Nette\Latte\Macros\CoreMacros
list($_l, $_g) = Nette\Latte\Macros\CoreMacros::initRuntime($template, 'eto7dklsyr')
;
// prolog Nette\Latte\Macros\UIMacros
//
// block content
//
if (!function_exists($_l->blocks['content'][] = '_lbc88e6aab35_content')) { function _lbc88e6aab35_content($_l, $_args) { extract($_args)
;call_user_func(reset($_l->blocks['title']), $_l, get_defined_vars())  ?>

	<div id="container">
	    <div id="sidebar">
        <div class="menu">
 	<ul>
<?php $iterations = 0; foreach ($menuItems as $item => $link): ?>		<li><a href="<?php echo htmlSpecialChars($_control->link($link)) ?>
"><?php echo Nette\Templating\Helpers::escapeHtml($item, ENT_NOQUOTES) ?></a></li>
<?php $iterations++; endforeach ?>	</ul>
        </div>
		</div>
		
<p><a href="<?php echo htmlSpecialChars($_control->link("add")) ?>">Nový prodej</a></p>

<table class="grid">
<tr>
	<th>Číslo</th>
	<th>Datum</th>
	<th>Exportováno</th>
        <th>Pojišťovna</th>
	<th>&nbsp;</th>
</tr>


<?php $iterations = 0; foreach ($prodejs as $prodej): ?>
<tr>
        <td><?php echo Nette\Templating\Helpers::escapeHtml($prodej->id, ENT_NOQUOTES) ?></td>
	<td><?php echo Nette\Templating\Helpers::escapeHtml($prodej->datum, ENT_NOQUOTES) ?></td>
        <td><?php echo Nette\Templating\Helpers::escapeHtml($prodej->exportovano, ENT_NOQUOTES) ?></td>
        <td><?php echo Nette\Templating\Helpers::escapeHtml($prodej->pojistovna_cislo ? $prodej->pojistovna_cislo : "Bez předpisu", ENT_NOQUOTES) ?></td>

	<td>
		<a href="<?php echo htmlSpecialChars($_control->link("detail", array($prodej->id))) ?>
">Detail</a>
		<a href="<?php echo htmlSpecialChars($_control->link("delete", array($prodej->id))) ?>
">Smazat</a>
	</td>
</tr>
<?php $iterations++; endforeach ?>
</table>



<!--
<table class="grid">
<tr>
	<th>Číslo</th>
	<th>Datum</th>
	<th>Lék</th>
	<th>&nbsp;</th>
</tr>


<?php $iterations = 0; foreach ($prodejs as $prodej): ?>
<tr>
        <td><?php echo Nette\Templating\Helpers::escapeHtmlComment($prodej->id) ?></td>
	<td><?php echo Nette\Templating\Helpers::escapeHtmlComment($prodej->datum) ?></td>
	
<?php $iterations = 0; foreach ($prodej->related('obsahoval') as $p): ?>
</tr><tr>
<td/><td/><td><?php echo Nette\Templating\Helpers::escapeHtmlComment($p->ref('lek', 'lek_kod')->nazev) ?></td>
</tr>

<?php $iterations++; endforeach ?>
        <td/><td/><td/>
	<td>
		<a n:href="edit, $prodej->id">Edit</a>
		<a n:href="delete, $prodej->id">Delete</a>
	</td>
</tr>
<?php $iterations++; endforeach ?>
</table>
--><?php
}}

//
// block title
//
if (!function_exists($_l->blocks['title'][] = '_lb1d8129fed3_title')) { function _lb1d8129fed3_title($_l, $_args) { extract($_args)
?><h1>Prodeje</h1>
<?php
}}

//
// end of blocks
//

// template extending and snippets support

$_l->extends = empty($template->_extended) && isset($_control) && $_control instanceof Nette\Application\UI\Presenter ? $_control->findLayoutTemplateFile() : NULL; $template->_extended = $_extended = TRUE;


if ($_l->extends) {
	ob_start();

} elseif (!empty($_control->snippetMode)) {
	return Nette\Latte\Macros\UIMacros::renderSnippets($_control, $_l, get_defined_vars());
}

//
// main template
//
?>

<?php if ($_l->extends) { ob_end_clean(); return Nette\Latte\Macros\CoreMacros::includeTemplate($_l->extends, get_defined_vars(), $template)->render(); }
call_user_func(reset($_l->blocks['content']), $_l, get_defined_vars()) ; 