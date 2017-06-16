<?php //netteCache[01]000387a:2:{s:4:"time";s:21:"0.20330500 1355063312";s:9:"callbacks";a:2:{i:0;a:3:{i:0;a:2:{i:0;s:19:"Nette\Caching\Cache";i:1;s:9:"checkFile";}i:1;s:65:"/home/users/xd/xdlapa01/WWW/IIS/app/templates/Prodej/detail.latte";i:2;i:1355060431;}i:1;a:3:{i:0;a:2:{i:0;s:19:"Nette\Caching\Cache";i:1;s:10:"checkConst";}i:1;s:25:"Nette\Framework::REVISION";i:2;s:30:"6a33aa6 released on 2012-10-01";}}}?><?php

// source file: /home/users/xd/xdlapa01/WWW/IIS/app/templates/Prodej/detail.latte

?><?php
// prolog Nette\Latte\Macros\CoreMacros
list($_l, $_g) = Nette\Latte\Macros\CoreMacros::initRuntime($template, 'vtdxylc60e')
;
// prolog Nette\Latte\Macros\UIMacros
//
// block content
//
if (!function_exists($_l->blocks['content'][] = '_lb0f53e22a71_content')) { function _lb0f53e22a71_content($_l, $_args) { extract($_args)
;$sum = 0 ;$dop = 0 ;call_user_func(reset($_l->blocks['title']), $_l, get_defined_vars())  ?>

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
            <th>Kód</th>
            <th>Název</th>
            <th>Cena</th>
            <th>Na předpis</th>
            <th>Doplatek</th>
           <!-- <th>&nbsp;</th>-->
        </tr>

<?php $iterations = 0; foreach ($leks as $lek): ?>
        <tr>
            <td><?php echo Nette\Templating\Helpers::escapeHtml($lek->kod, ENT_NOQUOTES) ?></td>
            <td><?php echo Nette\Templating\Helpers::escapeHtml($lek->ref('lek', 'lek_kod')->nazev, ENT_NOQUOTES) ?></td>
            <td><?php echo Nette\Templating\Helpers::escapeHtml($lek->ref('lek', 'lek_kod')->cena, ENT_NOQUOTES) ?></td>
            <td><?php echo Nette\Templating\Helpers::escapeHtml($lek->predepsany ? 'ano' : 'ne', ENT_NOQUOTES) ?></td>
            <td><?php echo Nette\Templating\Helpers::escapeHtml($lek->predepsany ? $lek->ref('lek', 'lek_kod')->doplatek : 0, ENT_NOQUOTES) ?></td>
            <!--
            <td>
                <a n:href="edit, $lek->kod">Edit</a>
                <a n:href="delete, $lek->kod">Delete</a>
            </td>-->
        </tr>
<?php $sum = $sum + $lek->ref('lek', 'lek_kod')->cena ;$lek->predepsany ? $dop += $lek->ref('lek', 'lek_kod')->doplatek : 'ne' ;$iterations++; endforeach ?>
        <tr>
            <td /><td />
            <td>
                Celková cena: <?php echo Nette\Templating\Helpers::escapeHtml($sum, ENT_NOQUOTES) ?>

            </td>
            <td>Celkový doplatek: <?php echo Nette\Templating\Helpers::escapeHtml($dop, ENT_NOQUOTES) ?>

            <td>Součet: <?php echo Nette\Templating\Helpers::escapeHtml($sum - $dop, ENT_NOQUOTES) ?>

        </tr>
    </table>
<?php
}}

//
// block title
//
if (!function_exists($_l->blocks['title'][] = '_lbf59b639f1f_title')) { function _lbf59b639f1f_title($_l, $_args) { extract($_args)
?><h1>Detail prodeje</h1>
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
if ($_l->extends) { ob_end_clean(); return Nette\Latte\Macros\CoreMacros::includeTemplate($_l->extends, get_defined_vars(), $template)->render(); }
call_user_func(reset($_l->blocks['content']), $_l, get_defined_vars()) ; 