<?php //netteCache[01]000388a:2:{s:4:"time";s:21:"0.24425400 1355063245";s:9:"callbacks";a:2:{i:0;a:3:{i:0;a:2:{i:0;s:19:"Nette\Caching\Cache";i:1;s:9:"checkFile";}i:1;s:66:"/home/users/xd/xdlapa01/WWW/IIS/app/templates/Import/default.latte";i:2;i:1354987642;}i:1;a:3:{i:0;a:2:{i:0;s:19:"Nette\Caching\Cache";i:1;s:10:"checkConst";}i:1;s:25:"Nette\Framework::REVISION";i:2;s:30:"6a33aa6 released on 2012-10-01";}}}?><?php

// source file: /home/users/xd/xdlapa01/WWW/IIS/app/templates/Import/default.latte

?><?php
// prolog Nette\Latte\Macros\CoreMacros
list($_l, $_g) = Nette\Latte\Macros\CoreMacros::initRuntime($template, 'q5vsuc6gnh')
;
// prolog Nette\Latte\Macros\UIMacros
//
// block content
//
if (!function_exists($_l->blocks['content'][] = '_lb565a753eb4_content')) { function _lb565a753eb4_content($_l, $_args) { extract($_args)
?>  <div id="container">
    <div id="sidebar">
      <div class="menu">
 	<ul>
<?php $iterations = 0; foreach ($menuItems as $item => $link): ?>	  <li><a href="<?php echo htmlSpecialChars($_control->link($link)) ?>
"><?php echo Nette\Templating\Helpers::escapeHtml($item, ENT_NOQUOTES) ?></a></li>
<?php $iterations++; endforeach ?>	</ul>
       </div>
     </div>

<?php call_user_func(reset($_l->blocks['title']), $_l, get_defined_vars())  ?>
<p> Import příspěvků na léky od pojišťoven pomocí CSV souboru, který má formát [kód léku],[nový doplatek]</p>

<table class="grid">
<?php $_ctrl = $_control->getComponent("importForm"); if ($_ctrl instanceof Nette\Application\UI\IRenderable) $_ctrl->validateControl(); $_ctrl->render() ?>
</table><?php
}}

//
// block title
//
if (!function_exists($_l->blocks['title'][] = '_lbf224e4e902_title')) { function _lbf224e4e902_title($_l, $_args) { extract($_args)
?><h1>Import příspěvků</h1>
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