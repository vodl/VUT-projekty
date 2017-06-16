<?php //netteCache[01]000389a:2:{s:4:"time";s:21:"0.71590500 1355063337";s:9:"callbacks";a:2:{i:0;a:3:{i:0;a:2:{i:0;s:19:"Nette\Caching\Cache";i:1;s:9:"checkFile";}i:1;s:67:"/home/users/xd/xdlapa01/WWW/IIS/app/templates/Export/exported.latte";i:2;i:1355062835;}i:1;a:3:{i:0;a:2:{i:0;s:19:"Nette\Caching\Cache";i:1;s:10:"checkConst";}i:1;s:25:"Nette\Framework::REVISION";i:2;s:30:"6a33aa6 released on 2012-10-01";}}}?><?php

// source file: /home/users/xd/xdlapa01/WWW/IIS/app/templates/Export/exported.latte

?><?php
// prolog Nette\Latte\Macros\CoreMacros
list($_l, $_g) = Nette\Latte\Macros\CoreMacros::initRuntime($template, '93zq7dowx6')
;
// prolog Nette\Latte\Macros\UIMacros
//
// block content
//
if (!function_exists($_l->blocks['content'][] = '_lb1b1a292977_content')) { function _lb1b1a292977_content($_l, $_args) { extract($_args)
;call_user_func(reset($_l->blocks['title']), $_l, get_defined_vars())  ?>

<div id="container">
    <div id="sidebar">
        <div class="menu">
            <ul>
<?php $iterations = 0; foreach ($menuItems as $item => $link): ?>                <li><a href="<?php echo htmlSpecialChars($_control->link($link)) ?>
"><?php echo Nette\Templating\Helpers::escapeHtml($item, ENT_NOQUOTES) ?></a></li>
<?php $iterations++; endforeach ?>            </ul>
        </div>
    </div>

    
<table class="grid">
<tr>
	<th>Číslo pojišťovny</th>
	<th>&nbsp;</th>
</tr>


<?php $iterations = 0; foreach ($arr['myArr'] as $arrPoj): ?>
<tr>
        <td><?php echo Nette\Templating\Helpers::escapeHtml(key($arr['myArr']), ENT_NOQUOTES) ?></td>
	<td>

        <table class="grid">
            <tr>
                <th>Datum a čas</th>
                <th>Kód léku</th>
                <th>Doplatek</th>
                <th>&nbsp;</th>
            </tr>

<?php $celkDopl = 0 ;$iterations = 0; foreach ($arrPoj as $arrProdej): ?>
            <tr>
                <td><?php echo Nette\Templating\Helpers::escapeHtml($prodejRep->findById(key($arr))->datum, ENT_NOQUOTES) ?></td>
                <td></td>
                <td></td>
                <td></td>
            </tr>

<?php $iterations = 0; foreach ($arrProdej as $lekCislo): ?>
            <tr>
                <td></td>
                <td><?php echo Nette\Templating\Helpers::escapeHtml($lekCislo, ENT_NOQUOTES) ?></td>
                <td><?php echo Nette\Templating\Helpers::escapeHtml($lekRep->findByID($lekCislo)->doplatek, ENT_NOQUOTES) ?></td>
                <td></td>
            </tr>
<?php $celkDopl += $lekRep->findByID($lekCislo)->doplatek ;$iterations++; endforeach ;$iterations++; endforeach ?>

            <tr>
                <td></td>
                <td></td>
                <td>Celkem: <?php echo Nette\Templating\Helpers::escapeHtml($celkDopl, ENT_NOQUOTES) ?></td>
                <td></td>
            </tr>
        </table>

</td></tr>
<?php $iterations++; endforeach ?>
</table>
<?php
}}

//
// block title
//
if (!function_exists($_l->blocks['title'][] = '_lb3af2eeaca5_title')) { function _lb3af2eeaca5_title($_l, $_args) { extract($_args)
?><h1>Export dokončen</h1>
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